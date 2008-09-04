Option Explicit
''
' This script updates the language PO files from the master POT file.
'
' Copyright (C) 2007 by Tim Gerundt
' Released under the "GNU General Public License"
'
' ID line follows -- this is updated by SVN
' $Id: UpdatePoFilesFromPotFile.vbs 4974 2008-01-29 23:10:28Z gerundt $

Const ForReading = 1

Dim oFSO, bRunFromCmd

Set oFSO = CreateObject("Scripting.FileSystemObject")

bRunFromCmd = False
If LCase(oFSO.GetFileName(Wscript.FullName)) = "cscript.exe" Then
  bRunFromCmd = True
End If

Call Main

''
' ...
Sub Main
  Dim oFiles, oFile, sFile, sDir, bPotChanged
  Dim oEnglishPotContent, oLanguagePoContent
  Dim StartTime, EndTime, Seconds
  Dim dDate, sBuild
  
  StartTime = Time
  
  InfoBox "Updating PO files from POT file...", 3
  
  sDir = oFSO.GetParentFolderName(Wscript.ScriptFullName)

  dDate = oFSO.GetFile(sDir & "\en-US.pot").DateLastModified
  sBuild = FormatTimeStamp(dDate) & ".UpdatePoFilesFromPotFile"

  Set oEnglishPotContent = GetContentFromPoFile(sDir & "\en-US.pot")
  If oEnglishPotContent.Count = 0 Then Err.Raise vbObjectError, "Sub Main", "Error reading content from en-US.pot"
  Set oFiles = Wscript.Arguments
  If oFiles.Count = 0 Then
    Set oFiles = oFSO.GetFolder(".").Files
    bPotChanged = Not oFSO.FileExists(sBuild)
  Else
    bPotChanged = True
  End If
  For Each oFile In oFiles 'For all languages...
    sFile = CStr(oFile)
    Select Case UCase(oFSO.GetExtensionName(sFile))
    Case "UPDATEPOFILESFROMPOTFILE"
      If oFile.Name <> sBuild Then oFile.Name = sBuild
      sBuild = ""
    Case "PO"
      If bPotChanged Or GetArchiveBit(sFile) Then 'If update necessary...
        If bRunFromCmd Then 'If run from command line...
          Wscript.Echo oFSO.GetFileName(sFile)
        End If
        Set oLanguagePoContent = GetContentFromPoFile(sFile)
        If oLanguagePoContent.Count > 0 Then 'If content exists...
          CreateUpdatedPoFile sFile, oEnglishPotContent, oLanguagePoContent
        End If
        SetArchiveBit sFile, False
      End If
    End Select
  Next
  If sBuild <> "" Then oFSO.CreateTextFile sBuild

  EndTime = Time
  Seconds = DateDiff("s", StartTime, EndTime)
  
  InfoBox "All PO files updated, after " & Seconds & " second(s).", 10
End Sub

''
' ...
Class CSubContent
  Dim sMsgCtxt2, sMsgId2, sMsgStr2, sTranslatorComments, sExtractedComments, sReferences, sFlags
End Class

''
' ...
Function GetContentFromPoFile(ByVal sPoPath)
  Dim oContent, oSubContent, oTextFile, sLine
  Dim oMatch, iMsgStarted, sMsgCtxt, sMsgId
  Dim reMsgCtxt, reMsgId, reMsgContinued

  Set reMsgCtxt = New RegExp
  reMsgCtxt.Pattern = "^msgctxt ""(.*)""$"
  reMsgCtxt.IgnoreCase = True

  Set reMsgId = New RegExp
  reMsgId.Pattern = "^msgid ""(.*)""$"
  reMsgId.IgnoreCase = True

  Set reMsgContinued = New RegExp
  reMsgContinued.Pattern = "^""(.*)""$"
  reMsgContinued.IgnoreCase = True
  
  Set oContent = CreateObject("Scripting.Dictionary")
  
  iMsgStarted = 0
  sMsgCtxt = ""
  Set oSubContent = New CSubContent
  Set oTextFile = oFSO.OpenTextFile(sPoPath, ForReading)
  Do Until oTextFile.AtEndOfStream 'For all lines...
    sLine = Trim(oTextFile.ReadLine)
    If sLine <> "" Then 'If NOT empty line...
      If Left(sLine, 1) <> "#" Then 'If NOT comment line...
        If reMsgCtxt.Test(sLine) Then 'If "msgctxt"...
          iMsgStarted = 1
          Set oMatch = reMsgCtxt.Execute(sLine)(0)
          sMsgCtxt = oMatch.SubMatches(0)
          oSubContent.sMsgCtxt2 = sLine & vbCrLf
        ElseIf reMsgId.Test(sLine) Then 'If "msgid"...
          iMsgStarted = 2
          Set oMatch = reMsgId.Execute(sLine)(0)
          sMsgId = oMatch.SubMatches(0)
          oSubContent.sMsgId2 = sLine & vbCrLf
        ElseIf Left(sLine, 8) = "msgstr """ Then 'If "msgstr"...
          iMsgStarted = 3
          oSubContent.sMsgStr2 = sLine & vbCrLf
        ElseIf reMsgContinued.Test(sLine) Then 'If "msgctxt", "msgid" or "msgstr" continued...
          If iMsgStarted = 1 Then
            sMsgCtxt = sMsgCtxt & oMatch.SubMatches(0)
            oSubContent.sMsgCtxt2 = oSubContent.sMsgCtxt2 & sLine & vbCrLf
          ElseIf iMsgStarted = 2 Then
            Set oMatch = reMsgContinued.Execute(sLine)(0)
            sMsgId = sMsgId & oMatch.SubMatches(0)
            oSubContent.sMsgId2 = oSubContent.sMsgId2 & sLine & vbCrLf
          ElseIf iMsgStarted = 3 Then
            oSubContent.sMsgStr2 = oSubContent.sMsgStr2 & sLine & vbCrLf
          End If
        End If
      Else 'If comment line...
        iMsgStarted = -1
        Select Case Left(sLine, 2)
          Case "#." 'Extracted comment...
            oSubContent.sExtractedComments = oSubContent.sExtractedComments & sLine & vbCrLf
          Case "#:" 'Reference...
            oSubContent.sReferences = oSubContent.sReferences & sLine & vbCrLf
          Case "#," 'Flag...
            oSubContent.sFlags = oSubContent.sFlags & sLine & vbCrLf
          Case Else 'Translator comment...
            oSubContent.sTranslatorComments = oSubContent.sTranslatorComments & sLine & vbCrLf
        End Select
      End If
    ElseIf iMsgStarted <> 0 Then 'If empty line AND there is pending translation...
      iMsgStarted = 0 'Don't process same translation twice
      If sMsgId = "" Then sMsgId = "__head__"
      oContent.Add sMsgCtxt & sMsgId, oSubContent
      sMsgCtxt = ""
      Set oSubContent = New CSubContent
    End If
  Loop
  oTextFile.Close
  Set GetContentFromPoFile = oContent
End Function

''
' ...
Sub CreateUpdatedPoFile(ByVal sPoPath, ByVal oEnglishPotContent, ByVal oLanguagePoContent)
  Dim sBakPath, oPoFile, sKey, oEnglish, oLanguage
  
  '--------------------------------------------------------------------------------
  ' Backup the old PO file...
  '--------------------------------------------------------------------------------
  sBakPath = sPoPath & ".bak"
  If oFSO.FileExists(sBakPath) Then
    oFSO.DeleteFile sBakPath
  End If
  oFSO.MoveFile sPoPath, sBakPath
  '--------------------------------------------------------------------------------
  
  Set oPoFile = oFSO.CreateTextFile(sPoPath, True)
  
  Set oLanguage = oLanguagePoContent("__head__")
  oPoFile.Write oLanguage.sTranslatorComments
  oPoFile.Write oLanguage.sMsgId2
  oPoFile.Write oLanguage.sMsgStr2
  oPoFile.Write vbCrLf
  For Each sKey In oEnglishPotContent.Keys 'For all English content...
    If sKey <> "__head__" Then
      Set oEnglish = oEnglishPotContent(sKey)
      If oLanguagePoContent.Exists(sKey) Then 'If translation exists...
        Set oLanguage = oLanguagePoContent(sKey)
      Else 'If translation NOT exists...
        Set oLanguage = oEnglish
      End If
      oPoFile.Write oLanguage.sTranslatorComments
      oPoFile.Write oEnglish.sExtractedComments
      oPoFile.Write oEnglish.sReferences
      oPoFile.Write oLanguage.sFlags
      oPoFile.Write oLanguage.sMsgCtxt2
      oPoFile.Write oLanguage.sMsgId2
      oPoFile.Write oLanguage.sMsgStr2
      oPoFile.Write vbCrLf
    End If
  Next
  oPoFile.Close
End Sub

''
' ...
Function FormatTimestamp(now)
  ' Form an ISO 8601 compliant timestamp without separators. Don't care about timezones.
  FormatTimestamp = Right("0000" & Year(now), 4) & Right("00" & Month(now), 2) & Right("00" & Day(now), 2) _
    & "T" & Right("00" & Hour(now), 2) & Right("00" & Minute(now), 2) & Right("00" & Second(now), 2)
End Function

''
' ...
Function InfoBox(ByVal sText, ByVal iSecondsToWait)
  Dim oShell
  
  If (bRunFromCmd = False) Then 'If run from command line...
    Set oShell = Wscript.CreateObject("WScript.Shell")
    InfoBox = oShell.Popup(sText, iSecondsToWait, Wscript.ScriptName, 64)
  Else 'If NOT run from command line...
    Wscript.Echo sText
  End If
End Function

''
' ...
Function GetArchiveBit(ByVal sFilePath)
  Dim oFile
  GetArchiveBit = False
  If oFSO.FileExists(sFilePath) Then 'If the file exists...
    Set oFile = oFSO.GetFile(sFilePath)
    If (oFile.Attributes And 32) = 32 Then 'If archive bit set...
      GetArchiveBit = True
    End If
  End If
End Function

''
' ...
Sub SetArchiveBit(ByVal sFilePath, ByVal bValue)
  Dim oFile
  If oFSO.FileExists(sFilePath) Then 'If the file exists...
    Set oFile = oFSO.GetFile(sFilePath)
    If bValue Xor (oFile.Attributes And 32) = 32 Then 'If archive bit different...
      oFile.Attributes = oFile.Attributes Xor 32
    End If
  End If
End Sub
