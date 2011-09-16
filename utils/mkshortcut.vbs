Set objWSHShell = CreateObject("WScript.Shell")
Set objShell = CreateObject("Shell.Application")
Set objFS = CreateObject("Scripting.FileSystemObject")

If WScript.Arguments.Count < 3 Then
	WScript.Quit 1
End If

sType = WScript.Arguments.Item(0)
sName = WScript.Arguments.Item(1)
sTargetPath = objWSHShell.ExpandEnvironmentStrings(WScript.Arguments.Item(2))

If StrComp(sType, "desktop") = 0 Then
	sBasePath = objShell.Namespace(&H0&).Self.Path
ElseIf StrComp(sType, "startmenu") = 0 Then
	sBasePath = objShell.Namespace(&H2&).Self.Path
Else
	WScript.Quit 2
End If

sShortCut = sBasePath & "\" & sName & ".lnk"

BuildFullPath ExtractDirName(sShortCut)

Set objShortCut = objWSHShell.CreateShortcut(sShortCut)
objShortCut.TargetPath = sTargetPath
objShortCut.WorkingDirectory = ExtractDirName(sTargetPath)
objShortCut.Save

Sub BuildFullPath(ByVal FullPath)
	arrNames = Split(strName, "\")
	intIndex = Ubound(arrNames)

	If Not objFS.FolderExists(FullPath) Then
		BuildFullPath objFS.GetParentFolderName(FullPath)
		objFS.CreateFolder FullPath
	End If
End Sub

Function ExtractDirName(ByVal Path)
	arrParts = Split(Path, "\")
	intUb = UBound(arrParts)
	if intUb > 0 Then
		redim preserve arrParts(intUb - 1)
	End If

	ExtractDirName = Join(arrParts, "\")
End Function