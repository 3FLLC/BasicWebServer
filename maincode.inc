/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure LoadContent(F:String);
Var
   BFH:File;
   Buffer:String;
   NumRead:Longint;

Begin
   AssignFile(BFH,F);
   Reset(BFH, 1);
   SetLength(Buffer, FileSize(BFH));
   While not EndOfFile(BFH) do begin
      BlockRead(BFH, Buffer[1], FileSize(BFH), NumRead);
      If NumRead>0 then Response.setContentBuffer(Buffer);
   End;
   SetLength(Buffer, 0);
   CloseFile(BFH);
End;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure Main;
var
   Path,Filename:String;
   LogStr:String;

Begin
   ServerRoot:=ExtractFilePath(ExecFilename);
LogStr:='* '+Session.getPeerIPAddress+#32;
   BuildRequest();
   lookupHost();
   Path:=buildDirectory(Request.getURI,FileName);
   If (FileName=="") then FileName:=HostRec.DefaultPage;
LogStr+="/"+Copy(Path,Length(ServerRoot)-1,255)+Filename+#32;
   If (Path!="") and (FileName!="") then begin
      If (FileExists(Path+FileName)) then Response.SetStatus(200)
      Else Response.SetStatus(404);
   End;
   If (Response.GetStatus==200) then LoadContent(Path+Filename);
   SendResponse(False, MimeEncoding(ExtractFileExtension(FileName)));
LogStr+=IntToStr(Response.getStatus)+' '+IntToStr(Length(Response.GetContentBuffer))+#13;
   Session.Disconnect;
Writeln(LogStr);
End;
