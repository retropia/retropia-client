If WScript.Arguments.Count >= 2 Then  
  Dim app, params, src, dst, exitCode, objShell, objWshShell

  src = WScript.Arguments(0)
  dst = WScript.Arguments(1)
  app = "cmd"
  params = "/c for /D %D IN (""" & src & "\*"") DO xcopy /Y /E /I /R ""%D"" """ & dst & """"

  Set objWshShell = WScript.CreateObject("WScript.Shell")
  Set objShell = CreateObject("Shell.Application")
  
  exitCode = objWshShell.Run(app & " " & params, 0, True)
  
  If exitCode <> 0 Then
  	objShell.ShellExecute app, params, "", "runas", 0
  End If

  Set objWshShell = Nothing
  Set objShell = Nothing

End If