{
    Copyright (c) 2002 by Marco van de Voort

    Header for syscall in system unit for i386 *BSD.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02111-1301, USA.

 ****************************************************************************

}

{$ifdef FPC_USE_SYSCALL}

Type

  TSysResult = longint; // all platforms, cint=32-bit.
                        // On platforms with off_t =64-bit, people should
                        // use int64, and typecast all calls that don't
                        // return off_t to cint.

// I don't think this is going to work on several platforms
// 64-bit machines don't have only 64-bit params.

  TSysParam  = Longint;

function do_sysCall(sysnr:TSysParam):TSysResult;oldfpccall;   external name 'FPC_DOSYS0';
function do_sysCall(sysnr,param1:TSysParam):TSysResult; oldfpccall;external name 'FPC_DOSYS1';
//function do_sysCall(sysnr,param1:integer):TSysResult; oldfpccall;external name 'FPC_DOSYS1w';
function do_sysCall(sysnr,param1,param2:TSysParam):TSysResult; oldfpccall; external name 'FPC_DOSYS2';
function do_sysCall(sysnr,param1,param2,param3:TSysParam):TSysResult;oldfpccall; external name 'FPC_DOSYS3';
function do_sysCall(sysnr,param1,param2,param3,param4:TSysParam):TSysResult;oldfpccall; external name 'FPC_DOSYS4';
function do_sysCall(sysnr,param1,param2,param3,param4,param5:TSysParam):TSysResult; oldfpccall;  external name 'FPC_DOSYS5';
function do_sysCall(sysnr,param1,param2,param3,param4,param5,param6:TSysParam):int64;oldfpccall;  external name 'FPC_DOSYS6';
function do_sysCall(sysnr,param1,param2,param3,param4,param5,param6,param7:TSysParam):int64; oldfpccall;  external name 'FPC_DOSYS7';
function do_sysCall(sysnr,param1,param2,param3,param4,param5,param6,param7,param8:TSysParam):int64; oldfpccall;  external name 'FPC_DOSYS8';


{$endif}

