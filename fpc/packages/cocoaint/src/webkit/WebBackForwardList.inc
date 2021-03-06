{ Parsed from Webkit.framework WebBackForwardList.h }

{$ifdef TYPES}
{$ifndef WEBBACKFORWARDLIST_PAS_T}
{$define WEBBACKFORWARDLIST_PAS_T}

{$endif}
{$endif}

{$ifdef RECORDS}
{$ifndef WEBBACKFORWARDLIST_PAS_R}
{$define WEBBACKFORWARDLIST_PAS_R}

{$endif}
{$endif}

{$ifdef FUNCTIONS}
{$ifndef WEBBACKFORWARDLIST_PAS_F}
{$define WEBBACKFORWARDLIST_PAS_F}

{$endif}
{$endif}

{$ifdef EXTERNAL_SYMBOLS}
{$ifndef WEBBACKFORWARDLIST_PAS_S}
{$define WEBBACKFORWARDLIST_PAS_S}

{$endif}
{$endif}

{$ifdef FORWARD}
  WebBackForwardList = objcclass;
  WebBackForwardListPointer = ^WebBackForwardList;
  WebBackForwardListPtr = WebBackForwardListPointer;

{$endif}

{$ifdef CLASSES}
{$ifndef WEBBACKFORWARDLIST_PAS_C}
{$define WEBBACKFORWARDLIST_PAS_C}

{ WebBackForwardList }
  WebBackForwardList = objcclass external (NSObject)
  private
    _private: WebBackForwardListPrivate;
    
  public
    procedure addItem(item: WebHistoryItem); message 'addItem:';
    procedure goBack; message 'goBack';
    procedure goForward; message 'goForward';
    procedure goToItem(item: WebHistoryItem); message 'goToItem:';
    function backItem: WebHistoryItem; message 'backItem';
    function currentItem: WebHistoryItem; message 'currentItem';
    function forwardItem: WebHistoryItem; message 'forwardItem';
    function backListWithLimit(limit: cint): NSArray; message 'backListWithLimit:';
    function forwardListWithLimit(limit: cint): NSArray; message 'forwardListWithLimit:';
    function capacity: cint; message 'capacity';
    procedure setCapacity(size: cint); message 'setCapacity:';
    function backListCount: cint; message 'backListCount';
    function forwardListCount: cint; message 'forwardListCount';
    function containsItem(item: WebHistoryItem): Boolean; message 'containsItem:';
    function itemAtIndex(index: cint): WebHistoryItem; message 'itemAtIndex:';
  end;

{ WebBackForwardListDeprecatedCategory }
  WebBackForwardListDeprecatedCategory = objccategory external (WebBackForwardList)
    procedure setPageCacheSize(size: WebNSUInteger); message 'setPageCacheSize:';
    function pageCacheSize: WebNSUInteger; message 'pageCacheSize';
  end;

{$endif}
{$endif}
