If WScript.Arguments.Count >= 2 Then  
  Dim app, params, src, dst, exitCode, objShell, objWshShell

  bat = WScript.Arguments(0)
  src = WScript.Arguments(1)
  dst = WScript.Arguments(2)
  cmd = "cmd"
  testparams = "/c copy NUL " & dst & "\install.log >NUL 2>&1"
  installparams = "/c " & bat & " " & src & " " & dst & " > " & dst & "\install.log 2>&1"

  Set objWshShell = WScript.CreateObject("WScript.Shell")
  Set objShell = CreateObject("Shell.Application")
  
  exitCode = objWshShell.Run(cmd & " " & testparams, 0, True)

  If exitCode <> 0 Then
    objShell.ShellExecute cmd, installparams, "", "runas", 0
  Else
    exitCode = objWshShell.Run(cmd & " " & installparams, 0, True)
  End If

  Set objWshShell = Nothing
  Set objShell = Nothing

End If