/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
function buildDirectory(URI:String;var FileName:String):String;
var
   Ws:String;
   Folders:TStringArray;

Begin
   FileName:='';
   If (Copy(HostRec.DomainRoot,Length(HostRec.DomainRoot),1)!=DirectorySeparator) then HostRec.DomainRoot+=DirectorySeparator;
   Result:=HostRec.DomainRoot;
   If (DirectorySeparator=='\') then URI:=StringReplace(URI,"/","\",[rfReplaceAll]);
   If (URI!=DirectorySeparator) then Begin
      Folders:=[];
      While URI!="" do begin
         Ws:=Fetch(URI,DirectorySeparator);
         If (Ws<>"") then begin
            if (Ws[1]==".") then begin // see if we are being hacked //
               If (Ws==".") then continue;
               If (Ws=="..") then begin
                  If (Length(Folders)>0) then SetLength(Folders,Length(Folders)-1);
                  Continue;
               End;
            end;
            ArrayPush(Ws,Folders);
         End;
      End;
      Ws:=HostRec.DomainRoot;
      For Var Loop:=Low(Folders) to High(Folders) do begin
         If DirExists(Ws+Folders[Loop]+DirectorySeparator) then begin
            Ws:=Ws+Folders[Loop]+DirectorySeparator;
            Result:=Ws;
         End
         Else Begin
            If (Loop==high(Folders)) then begin // it is a filename //
               Filename:=Folders[Loop];
            end
            else begin
               Result:='';
               Exit;
            End;
         End;
      End;
   End;
End;
