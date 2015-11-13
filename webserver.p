/////////////////////////////////////////////////////////////////////////////
// Winshock Web Server, v3.0 (basic, not my full engine)
// Author: G.E. Ozz Nixon Jr.
// ==========================================================================
// WWS was originally written using Delphi 7 using DXSock. This is a manual
// port of the code to Modern Pascal, which has DXSock built-in, along with
// the ability to run behind CodeRunner which is a DXSock Listener that has
// Modern Pascal built-in.
// --------------------------------------------------------------------------
// Copyright: (c) 2015 by Winshock.com
/////////////////////////////////////////////////////////////////////////////

uses
   Environment, // File and Folder methods
   IniFiles,    // used as conf files
   HTTP;        // Request and Response Objects

{$Include_Once struct.inc}
{$Include_Once globals.inc}
{$Include_Once buildhttp.inc}
{$Include_Once virthosts.inc}
{$Include_Once directories.inc}

{$Include_Once maincode.inc}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure Initialization;
begin
   rawHTTPHeader.Init();
   Request.Init();
   Response.Init();
end;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure Finalization;
begin
   Response.Free;
   Request.Free;
   rawHTTPHeader.Free;
end;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
begin
   Initialization;
try
   Main;
finally
   Finalization;
end;
end.
