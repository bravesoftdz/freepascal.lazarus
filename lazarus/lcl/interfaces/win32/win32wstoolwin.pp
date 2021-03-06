{ $Id: win32wstoolwin.pp 41387 2013-05-24 18:30:06Z juha $}
{
 *****************************************************************************
 *                             Win32WSToolwin.pp                             * 
 *                             -----------------                             * 
 *                                                                           *
 *                                                                           *
 *****************************************************************************

 *****************************************************************************
  This file is part of the Lazarus Component Library (LCL)

  See the file COPYING.modifiedLGPL.txt, included in this distribution,
  for details about the license.
 *****************************************************************************
}
unit Win32WSToolwin;

{$mode objfpc}{$H+}

interface

uses
////////////////////////////////////////////////////
// I M P O R T A N T                                
////////////////////////////////////////////////////
// To get as little as posible circles,
// uncomment only when needed for registration
////////////////////////////////////////////////////
//  Toolwin,
////////////////////////////////////////////////////
  WSToolwin, WSLCLClasses;

type

  { TWin32WSToolWindow }

  TWin32WSToolWindow = class(TWSToolWindow)
  published
  end;


implementation

initialization

////////////////////////////////////////////////////
// I M P O R T A N T
////////////////////////////////////////////////////
// To improve speed, register only classes
// which actually implement something
////////////////////////////////////////////////////
//  RegisterWSComponent(TToolWindow, TWin32WSToolWindow);
////////////////////////////////////////////////////
end.
