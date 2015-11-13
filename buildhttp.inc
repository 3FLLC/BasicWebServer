/////////////////////////////////////////////////////////////////////////////
// Ability to read from the socket the HTTP request and populate a structure
// of what we need for the game. CodeRunner allows you to interact with the
// connection below a protocl - so this file implements all of the protocol
// pieces associated with the HTTP protocol (just what is needed).
// --------------------------------------------------------------------------
// Design benefit, since Modern Pascal's Code Runner handles the threading
// for you, all of your coded is written as if it was single-user. This means
// you get to benefit by using global variables which are only global to the
// current connection not other connections.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure BuildRequest;
var
   Ws:String;

procedure LoadPostData;
var
   Ws:String;
   Timeout:Word;
   Len:LongWord;

begin
   Ws:='';
   Timeout:=0;
   if Request.getContentLength=0 then Exit;
   While Session.IsConnected do begin
      If Session.IsReadable then begin
         If Session.CountWaiting=0 then Break;
         Ws+=Session.ReadStr(Session.CountWaiting,100);
         If Length(Ws)>=Request.getContentLength then Break;
      end
      else begin
         Yield(1);
         Inc(Timeout);
         If Timeout>12000 then exit; // too damn slow, abort!
      end;
   end;
   Request.setPostData(Copy(Ws,1,Request.getContentLength));
end;

begin
{$IFDEF TRACECODE}Writeln('@CollectHeader');{$ENDIF}
   While Session.IsConnected do begin          // While browser is connected, loop
      If Session.IsReadable then begin         // If we received something from the browser/network
         If Session.CountWaiting=0 then Break; // If SYN_FIN packet (0 bytes) we are disconnected
         Ws:=Session.Readln(100);              // Upto 100ms or CR/LF read a string from the network
         If Ws<>'' then rawHTTPHeader.add(Ws)  // HTTP sends HEADER <blank line> BODY - is there data?
         Else Break;                           // Nope, then exit - end of HEADER.
      end
      else Yield(1);                           // CPU friendly, give up 1ms to other applications
   end;

{$IFDEF TRACECODE}Writeln('@ProcessRequest');{$ENDIF}
   Ws:=rawHTTPHeader.getStrings(0);           // First line of HTTP Header is GET / HTTP/1.0 (for example)
   rawHTTPHeader.setText(StringReplace(rawHTTPHeader.getText(),': ','=',[rfReplaceAll]));
   with Request do begin

      setContentLength(StrToIntDef(rawHTTPHeader.getValues("Content-Length"),0));
      setRequestLine(Ws);
      setRequestTime(IntToStr(Timestamp));

      setMethod(Fetch(Ws,#32));
      setURI(Fetch(Ws,#32));
      setProtocol(Ws);

//    addClassVar('TCelerityRequest', 'ServerBanner
//    addClassVar('TCelerityRequest', 'ServerBuild
//    addClassVar('TCelerityRequest', 'ServerDescription
//    addClassVar('TCelerityRequest', 'ServerVersion
      setAccept(rawHTTPHeader.getValues("Accept"));
      setAcceptCharset(rawHTTPHeader.getValues("Accept-Charset"));
      setAcceptDatetime(rawHTTPHeader.getValues("Accept-Datetime"));
      setAcceptEncoding(rawHTTPHeader.getValues("Accept-Encoding"));
      setAcceptLanguage(rawHTTPHeader.getValues("Accept-Language"));
      setAuthorization(rawHTTPHeader.getValues("Authorization"));
      setAuthType(rawHTTPHeader.getValues("AuthType"));
      setBoundary(rawHTTPHeader.getValues("Boundary"));
      setCacheControl(rawHTTPHeader.getValues("Cache-Control"));
      setConnection(rawHTTPHeader.getValues("Connection"));
      setContentDisposition(rawHTTPHeader.getValues("Content-Disposition"));
      setContentEncoding(rawHTTPHeader.getValues("Content-Encoding"));
      setContentMD5(rawHTTPHeader.getValues("Content-MD5"));
      setCookie(rawHTTPHeader.getValues("Cookie"));
      setContentType(rawHTTPHeader.getValues("Content-Type"));
      setDate(rawHTTPHeader.getValues("Date"));
      setDigest(rawHTTPHeader.getValues("Digest"));
//    addClassVar('TCelerityRequest', 'DomainRoot
//    addClassVar('TCelerityRequest', 'Filename
      setFrom(rawHTTPHeader.getValues("From"));
      setFrontEndHTTPS(rawHTTPHeader.getValues("Front-End-HTTPS"));
      setHost(rawHTTPHeader.getValues("Host"));
      setIfMatch(rawHTTPHeader.getValues("If-Match"));
      setIfModifiedSince(rawHTTPHeader.getValues("If-Modified-Since"));
      setIfNoneMatch(rawHTTPHeader.getValues("If-None-Match"));
      setIfNoneRange(rawHTTPHeader.getValues("If-None-Range"));
      setIfUnmodifiedSince(rawHTTPHeader.getValues("If-Unmodified-Since"));
      setMaxForwards(rawHTTPHeader.getValues("Max-Forwards"));
      setOrigin(rawHTTPHeader.getValues("Origin"));
      setPath(rawHTTPHeader.getValues("Path"));
//    addClassVar('TCelerityRequest', 'PeerIP
//    addClassVar('TCelerityRequest', 'PeerHost
      If Pos(":",getHost())>0 then begin
         setPort(Copy(getHost,Pos(":",getHost)+1,255));
         setHost(Copy(getHost,1,Pos(":",getHost)-1));
      End
      Else setPort(IntToStr(Session.getLocalPort));
      setPragma(rawHTTPHeader.getValues("Pragma"));
      setProxyAuthorization(rawHTTPHeader.getValues("Proxy-Authorization"));
      setProxyConnection(rawHTTPHeader.getValues("Proxy-Connection"));
//    addClassVar('TCelerityRequest', 'QueryString
      setRange(rawHTTPHeader.getValues("Range"));
      setReferer(rawHTTPHeader.getValues("Referer"));
//    addClassVar('TCelerityRequest', 'ServerIP
//    addClassVar('TCelerityRequest', 'StatusLine
      setUpgrade(rawHTTPHeader.getValues("Upgrade"));
      setUser(rawHTTPHeader.getValues("User"));
      setUserAgent(rawHTTPHeader.getValues("User-Agent"));
      setVia(rawHTTPHeader.getValues("Via"));
      setWarning(rawHTTPHeader.getValues("Warning"));
      setOriginalXML(rawHTTPHeader.getValues("Original-XML"));
   end;
   if Request.getContentLength>0 then LoadPostData;
end;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
function ResponseCode(Code:Word):String;
begin
   Case Code of
      100:Result:='Continue';
      101:Result:='Switching Protocol';
      102:Result:='Processing';

      200:Result:='OK';
      201:Result:='Created';
      202:Result:='Accepted';
      203:Result:='Non-Authoritative Information';
      204:Result:='No Content';
      205:Result:='Reset Content';
      206:Result:='Partial Content';
      207:Result:='Multi-Status';     // (WebDAV)
      208:Result:='Already Reported'; // (WebDAV)
      226:Result:='IM Used';

      300:Result:='Multiple Choices';
      301:Result:='Moved Permanently';
      302:Result:='Found';
      303:Result:='Method';
      304:Result:='Not Modified';
      305:Result:='Use Proxy';
      306:Result:='(Unused)';
      307:Result:='Temporary Redirect';
      308:Result:='Permanent Redirect'; // (experiemental)

      400:Result:='Bad Request';
      401:Result:='Unauthorized';
      402:Result:='Payment Required';
      403:Result:='Forbidden';
      404:Result:='Not Found';
      405:Result:='Method Not Allowed';
      406:Result:='Not Acceptable';
      408:Result:='Request Timeout';
      409:Result:='Conflict';
      410:Result:='Gone';
      411:Result:='Length Required';
      412:Result:='Precondition Failed';
      413:Result:='Payload Too Large';
      414:Result:='URI Too Long';
      415:Result:='Unsupported Media Type';
      417:Result:='Expectation Failed';
      418:Result:='I'+#39+'m a teapot'; // (RFC 2324)
      420:Result:='Enhance Your Calm'; // (Twitter)
      422:Result:='Unprocessable Entity'; // (WebDAV)
      423:Result:='Locked'; // (WebDAV)
      424:Result:='Failed Dependency'; // (WebDAV)
      425:Result:='Reserved for WebDAV';
      426:Result:='Upgrade Required';
      428:Result:='Precondition Required';
      429:Result:='Too Many Requests';
      431:Result:='Request Header Fields Too Large';
      444:Result:='No Response'; // (Nginx)
      449:Result:='Retry With'; // (Microsoft)
      450:Result:='Blocked by Windows Parental Controls'; // (Microsoft)
      499:Result:='Client Closed Request'; // (Nginx)

      500:Result:='Internal Server Error';
      501:Result:='Not Implemented';
      502:Result:='Bad Gateway';
      503:Result:='Service Unavailable';
      504:Result:='Gateway Timeout';
      505:Result:='HTTP Version Not Supported';
      506:Result:='Variant Also Negotiates'; // (Experimental)
      507:Result:='Insufficient Storage'; // (WebDAV)
      508:Result:='Loop Detected'; // (WebDAV)
      509:Result:='Bandwidth Limit Exceeded'; // (Apache)
      510:Result:='Not Extended';
      511:Result:='Network Authentication Required';
      598:Result:='Network read timeout error';
      599:Result:='Network connect timeout error';

/////////////////////////////////////////////////////////////////////////////
// RESTFul API: 200, 201, 204, 304, 400, 401, 403, 404, 409, 500
/////////////////////////////////////////////////////////////////////////////

   End;
   Result:=IntToStr(Code)+#32+Result;
End;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
function MimeEncoding(FileExtension:String):string;
var
   MimeTypes:TIniFile;
Begin
   If FileExtension='' then Result:='text/plain'
   else begin
      If FileExtension[1]='.' then Delete(FileExtension,1,1);
      MimeTypes.Init(ServerRoot+"mimetypes.db");
      Result:=MimeTypes.ReadString(FileExtension,"R","text/plain");
      MimeTypes.Free;
   end;
end;

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
procedure SendResponse(ConnBool:Boolean;MimeStr:String);
Const
   ConnectionStr:Array[false..true] of String=['close','keep-alive'];

Begin
   If (Response.GetStatus==200) and (Length(Response.GetContentBuffer)=0) then Response.SetStatus(204);
   Session.Writeln(Request.GetProtocol+#32+ResponseCode(Response.GetStatus)+LineEnding+
      'Access-Control-Allow-Origin: *'+LineEnding+
      'Allow: GET, HEAD, POST, OPTIONS'+LineEnding+
      'Connection: '+ConnectionStr[ConnBool]+LineEnding+
      'Content-Type: '+MimeStr+LineEnding+
      'Expires: Thu, 01 Dec 1994 16:00:00 GMT'+LineEnding+
      'Cache-Control: no-store'+LineEnding+
      'Content-Length: '+IntToStr(Length(Response.getContentBuffer))+LineEnding+
      'Pragma: no-cache'+LineEnding+
//      'WWW-Authentication: Basic'+LineEnding+
      'Server: '+__SERVERNAME__+' (CodeRunner)'+LineEnding+
      'X-Powered-by: webserver.p'+LineEnding);
   If Length(Response.GetContentBuffer)>0 then Session.Write(Response.GetContentBuffer);
End;
