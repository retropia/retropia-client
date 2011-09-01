If WScript.Arguments.Count >= 2 Then  
  Dim app, params, src, dst, exitCode, objShell, objWshShell

  src = WScript.Arguments(0)
  dst = WScript.Arguments(1)
  app = "cmd"
  testparams = "/c copy /Y NUL > install.log >NUL 2>&1"
  installparams = "/c utils\install.bat """ & src & """ """ & dst & """ > install.log 2>&1"

  Set objWshShell = WScript.CreateObject("WScript.Shell")
  Set objShell = CreateObject("Shell.Application")
  
  exitCode = objWshShell.Run(app & " " & testparams, 0, True)

  If exitCode <> 0 Then
    objShell.ShellExecute app, installparams, "", "runas", 0
  Else
    exitCode = objWshShell.Run(app & " " & installparams, 0, True)
  End If

  Set objWshShell = Nothing
  Set objShell = Nothing

End If