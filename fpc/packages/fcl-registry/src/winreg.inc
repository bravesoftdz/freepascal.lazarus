Const
  RegDataWords : Array [TRegDataType] of DWORD
               = (REG_NONE,REG_SZ,REG_EXPAND_SZ,REG_BINARY,REG_DWORD,REG_DWORD_BIG_ENDIAN,
                  REG_LINK,REG_MULTI_SZ,REG_RESOURCE_LIST,REG_FULL_RESOURCE_DESCRIPTOR,REG_RESOURCE_REQUIREMENTS_LIST);

type
  TWinRegData = record
    RootKeyOwned: Boolean;
  end;
  PWinRegData = ^TWinRegData;

{******************************************************************************
                                  TRegistry
 ******************************************************************************}

Procedure TRegistry.SysRegCreate;
begin
  FStringSizeIncludesNull:=True;
  New(PWinRegData(FSysData));
  PWinRegData(FSysData)^.RootKeyOwned := False;
end;

Procedure TRegistry.SysRegfree;

begin
  if PWinRegData(FSysData)^.RootKeyOwned and (RootKey <> 0) then
    RegCloseKey(RootKey);
  Dispose(PWinRegData(FSysData));
end;

Function PrepKey(Const S : String) : pChar;

begin
  Result:=PChar(S);
  If Result^='\' then
    Inc(Result);
end;

Function RelativeKey(Const S : String) : Boolean;

begin
  Result:=(S='') or (S[1]<>'\')
end;


function TRegistry.sysCreateKey(const Key: String): Boolean;
Var
  u: UnicodeString;
  Disposition: Dword;
  Handle: HKEY;
  SecurityAttributes: Pointer; //LPSECURITY_ATTRIBUTES;

begin
  SecurityAttributes := Nil;
  u:=UTF8Decode(PrepKey(Key));
  FLastError:=RegCreateKeyExW(GetBaseKey(RelativeKey(Key)),
                              PWideChar(u),
                              0,
                              '',
                              REG_OPTION_NON_VOLATILE,
                              KEY_ALL_ACCESS,
                              SecurityAttributes,
                              Handle,
                              @Disposition);
  Result:=FLastError=ERROR_SUCCESS;
  RegCloseKey(Handle);
end;

function TRegistry.DeleteKey(const Key: String): Boolean;

Var
  u: UnicodeString;
begin
  u:=UTF8Decode(PRepKey(Key));
  FLastError:=RegDeleteKeyW(GetBaseKey(RelativeKey(Key)),PWideChar(u));
  Result:=FLastError=ERROR_SUCCESS;
end;

function TRegistry.DeleteValue(const Name: String): Boolean;
begin
  FLastError:= RegDeleteValueW(fCurrentKey, PWideChar(UTF8Decode(Name)));
  Result:=FLastError=ERROR_SUCCESS;
end;

function TRegistry.SysGetData(const Name: String; Buffer: Pointer;
          BufSize: Integer; Out RegData: TRegDataType): Integer;
Var
  u: UnicodeString;
  RD : DWord;

begin
  u := UTF8Decode(Name);
  FLastError:=RegQueryValueExW(fCurrentKey,PWideChar(u),Nil,
                      @RD,Buffer,lpdword(@BufSize));
  if (FLastError<>ERROR_SUCCESS) Then
    Result:=-1
  else
    begin
    RegData:=High(TRegDataType);
    While (RegData>rdUnknown) and (RD<>RegDataWords[RegData]) do
      RegData:=Pred(RegData);
    Result:=BufSize;
    end;
end;

function TRegistry.GetDataInfo(const ValueName: String; out Value: TRegDataInfo): Boolean;

Var
  u: UnicodeString;
  RD : DWord;

begin
  u:=UTF8Decode(ValueName);
  With Value do
    begin
    FLastError:=RegQueryValueExW(fCurrentKey,PWideChar(u),Nil,lpdword(@RegData),Nil,lpdword(@DataSize));
    Result:=FLastError=ERROR_SUCCESS;
    if Result then
      begin
      RD:=DWord(RegData);
      RegData:=High(TRegDataType);
      While (RegData>rdUnknown) and (RD<>RegDataWords[RegData]) do
         RegData:=Pred(RegData);
      end;
    end;
  If Not Result Then
    begin
    Value.RegData := rdUnknown;
    Value.DataSize := 0
    end
end;


function TRegistry.GetKey(const Key: String): HKEY;
var
  S : string;
{$ifndef WinCE}
  u : UnicodeString;
{$endif}
  Rel : Boolean;
begin
  Result:=0;
  S:=Key;
  Rel:=RelativeKey(S);
  if not(Rel) then
    Delete(S,1,1);
{$ifdef WinCE}
  FLastError:=RegOpenKeyEx(GetBaseKey(Rel),PWideChar(WideString(S)),0,FAccess,Result);
{$else WinCE}
  u:=UTF8Decode(S);
  FLastError:=RegOpenKeyExW(GetBaseKey(Rel),PWideChar(u),0,FAccess,Result);
{$endif WinCE}
end;


function TRegistry.GetKeyInfo(out Value: TRegKeyInfo): Boolean;
var
  winFileTime: Windows.FILETIME;
  sysTime: TSystemTime;
begin
  FillChar(Value, SizeOf(Value), 0);
  With Value do
    begin
    FLastError:=RegQueryInfoKeyA(CurrentKey,nil,nil,nil,lpdword(@NumSubKeys),
              lpdword(@MaxSubKeyLen),nil,lpdword(@NumValues),lpdword(@MaxValueLen),
              lpdword(@MaxDataLen),nil,@winFileTime);
    Result:=FLastError=ERROR_SUCCESS;          
    end;          
  if Result then
  begin
    FileTimeToSystemTime(@winFileTime, @sysTime);
    Value.FileTime := SystemTimeToDateTime(sysTime);
  end;
end;


function TRegistry.KeyExists(const Key: string): Boolean;
var
  KeyHandle : HKEY;
  OldAccess : LONG;
begin
  Result:=false;
  OldAccess:=FAccess;
  try
    FAccess:=KEY_QUERY_VALUE or KEY_ENUMERATE_SUB_KEYS or STANDARD_RIGHTS_READ 
    {$ifndef WinCE} or (OldAccess and (KEY_WOW64_64KEY or KEY_WOW64_32KEY)) {$endif};
    KeyHandle:=GetKey(Key);
    if KeyHandle<>0 then
      begin
        RegCloseKey(KeyHandle);
        Result:=true;
      end;
  finally
    FAccess:=OldAccess;
  end;
end;


function TRegistry.LoadKey(const Key, FileName: string): Boolean;
begin
  Result := False;
end;


function TRegistry.OpenKey(const Key: string; CanCreate: Boolean): Boolean;

Var
  u: UnicodeString;
  Handle: HKEY;
  Disposition: Integer;
  SecurityAttributes: Pointer; //LPSECURITY_ATTRIBUTES;
  S: string;
begin
  SecurityAttributes := Nil;
  u:=UTF8Decode(PrepKey(Key));
  If CanCreate then
    begin
    Handle:=0;
    FLastError:=RegCreateKeyExW(GetBaseKey(RelativeKey(Key)),PWideChar(u),0,'',
                           REG_OPTION_NON_VOLATILE,
                           fAccess,SecurityAttributes,Handle,
                           pdword(@Disposition));
    Result:=FLastError=ERROR_SUCCESS;
    end
  else
    begin
    FLastError:=RegOpenKeyExW(GetBaseKey(RelativeKey(Key)),
                              PWideChar(u),0,fAccess,Handle);
    Result:=FLastError=ERROR_SUCCESS;
    end;                     
  If Result then begin
    if RelativeKey(Key) then
      S:=CurrentPath + Key
    else
      S:=UTF8Encode(u);
    ChangeKey(Handle, S);
  end;
end;

function TRegistry.OpenKeyReadOnly(const Key: string): Boolean;

Var
  OldAccess: LongWord;
begin
  OldAccess:=fAccess;
  fAccess:=KEY_READ {$ifndef WinCE} or (OldAccess and (KEY_WOW64_64KEY or KEY_WOW64_32KEY)) {$endif};
  try
    Result:=OpenKey(Key, False);
  finally
    fAccess:=OldAccess;
  end;
end;

function TRegistry.RegistryConnect(const UNCName: string): Boolean;
{$ifndef WinCE}
var
  newroot: HKEY;
{$endif}
begin
{$ifdef WinCE}
  Result:=False;
{$else}
  FLastError:=RegConnectRegistryW(PWideChar(UTF8Decode(UNCName)),RootKey,newroot);
  Result:=FLastError=ERROR_SUCCESS;
  if Result then begin
    RootKey:=newroot;
    PWinRegData(FSysData)^.RootKeyOwned:=True;
  end;
{$endif}
end;

function TRegistry.ReplaceKey(const Key, FileName, BackUpFileName: string): Boolean;
begin
  Result := False;
end;

function TRegistry.RestoreKey(const Key, FileName: string): Boolean;
begin
  Result := False;
end;

function TRegistry.SaveKey(const Key, FileName: string): Boolean;
begin
  Result := False;
end;

function TRegistry.UnLoadKey(const Key: string): Boolean;
begin
  Result := false;
end;

function TRegistry.ValueExists(const Name: string): Boolean;

var
  Info : TRegDataInfo;

begin
  Result:=GetDataInfo(Name,Info);
end;

procedure TRegistry.CloseKey;
begin
  If (CurrentKey<>0) then
    begin
    if LazyWrite then
      RegCloseKey(CurrentKey)
    else
      RegFlushKey(CurrentKey);
    fCurrentKey:=0;
    end;
  fCurrentPath:='';
end;

procedure TRegistry.CloseKey(key:HKEY);
begin
  RegCloseKey(key);
end;

procedure TRegistry.ChangeKey(Value: HKey; const Path: String);
begin
  CloseKey;
  FCurrentKey:=Value;
  FCurrentPath:=Path;
end;

procedure TRegistry.GetKeyNames(Strings: TStrings);

var
  Info:    TRegKeyInfo;
  dwLen:   DWORD;
  lpName:  LPWSTR;
  dwIndex: DWORD;
  lResult: LONGINT;
  s:       string;

begin
  Strings.Clear;
  if GetKeyInfo(Info) then
  begin
    dwLen:=Info.MaxSubKeyLen+1;
    GetMem(lpName,dwLen*SizeOf(WideChar));
    try
      for dwIndex:=0 to Info.NumSubKeys-1 do
      begin
        dwLen:=Info.MaxSubKeyLen+1;
        lResult:=RegEnumKeyExW(CurrentKey,dwIndex,lpName,dwLen,Nil,Nil,Nil,Nil);
        if lResult<>ERROR_SUCCESS then
          raise ERegistryException.Create(SysErrorMessage(lResult));
        if dwLen=0 then
          s:=''
        else
        begin           // dwLen>0
          SetLength(s,dwLen*3);
          dwLen:=UnicodeToUTF8(PChar(s),Length(s)+1,lpName,dwLen);
          if dwLen<=1 then
            s:=''
          else          // dwLen>1
            SetLength(s,dwLen-1);
        end;            // if dwLen=0
        Strings.Add(s);
      end;              // for dwIndex:=0 ...

    finally
      FreeMem(lpName);
    end;
  end;
end;

procedure TRegistry.GetValueNames(Strings: TStrings);

var
  Info:    TRegKeyInfo;
  dwLen:   DWORD;
  lpName:  LPWSTR;
  dwIndex: DWORD;
  lResult: LONGINT;
  s:       string;

begin
   Strings.Clear;
  if GetKeyInfo(Info) then
  begin
    dwLen:=Info.MaxValueLen+1;
    GetMem(lpName,dwLen*SizeOf(WideChar));
    try
      for dwIndex:=0 to Info.NumValues-1 do
      begin
        dwLen:=Info.MaxValueLen+1;
        lResult:=RegEnumValueW(CurrentKey,dwIndex,lpName,dwLen,Nil,Nil,Nil,Nil);
        if lResult<>ERROR_SUCCESS then
          raise ERegistryException.Create(SysErrorMessage(lResult));
        if dwLen=0 then
          s:=''
        else
        begin           // dwLen>0
          SetLength(s,dwLen*3);
          dwLen:=UnicodeToUTF8(PChar(s),Length(s)+1,lpName,dwLen);
          if dwLen<=1 then
            s:=''
          else          // dwLen>1
            SetLength(s,dwLen-1);
        end;            // if dwLen=0
        Strings.Add(s);
      end;              // for dwIndex:=0 ...

    finally
      FreeMem(lpName);
    end;
  end;
end;


Function TRegistry.SysPutData(const Name: string; Buffer: Pointer;
  BufSize: Integer; RegData: TRegDataType) : Boolean;


Var
  u: UnicodeString;
  RegDataType: DWORD;
  B : Pchar;
  S : String;

begin
  RegDataType:=RegDataWords[RegData];
  u:=UTF8Decode(Name);
  FLastError:=RegSetValueExW(fCurrentKey,PWideChar(u),0,RegDataType,Buffer,BufSize);
  Result:=FLastError=ERROR_SUCCESS;
end;

procedure TRegistry.RenameValue(const OldName, NewName: string);

var
  L: Integer;
  InfoO,InfoN : TRegDataInfo;
  D : TRegDataType;
  P: PChar;

begin
  If GetDataInfo(OldName,InfoO) and Not GetDataInfo(NewName,InfoN) then
    begin
    L:=InfoO.DataSize;
    if L>0 then
      begin
      GetMem(P,L);
      try
        L:=GetData(OldName,P,L,D);
        If SysPutData(NewName,P,L,D) then
          DeleteValue(OldName);
      finally
        FreeMem(P);
      end;
      end;
    end;
end;

procedure TRegistry.SetCurrentKey(Value: HKEY);
begin
  fCurrentKey := Value;
end;

procedure TRegistry.SetRootKey(Value: HKEY);
begin
  if fRootKey = Value then
    Exit;
  { close a root key that was opened using RegistryConnect }
  if PWinRegData(FSysData)^.RootKeyOwned and (fRootKey <> 0) then begin
    RegCloseKey(fRootKey);
    PWinRegData(FSysData)^.RootKeyOwned := False;
  end;
  fRootKey := Value;
end;

function TRegistry.GetLastErrorMsg: string;
begin
  if FLastError <> ERROR_SUCCESS then
    Result:=SysErrorMessage(FLastError)
  else
    Result:='';
end;

