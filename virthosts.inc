/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
function lookupHost():Boolean;
var
   Hosts:TIniFile;
   Ws:String;

Begin
   Hosts.Init(ServerRoot+"hosts.db");
   If (Request.getHost() != "") then begin
      If (Hosts.ReadString("global",Request.getHost(),"")!="") then begin
         Ws:=Hosts.ReadString("global",Request.getHost(),"");
         HostRec.Host:=Request.getHost();
         HostRec.DomainRoot:=Hosts.ReadString(Ws,"DomainRoot","");
         HostRec.DefaultPage:=Hosts.ReadString(Ws,"DefaultPage","index.html");
      End
      Else Begin
         HostRec.Host:=Hosts.ReadString("default","domain","");
         HostRec.DomainRoot:=Hosts.ReadString("default","DomainRoot","");
         HostRec.DefaultPage:=Hosts.ReadString("default","DefaultPage","index.html");
      End;
   End
   Else Begin
      HostRec.Host:=Hosts.ReadString("default","domain","");
      HostRec.DomainRoot:=Hosts.ReadString("default","DomainRoot","");
      HostRec.DefaultPage:=Hosts.ReadString("default","DefaultPage","index.html");
   End;
   Hosts.Free;
End;
