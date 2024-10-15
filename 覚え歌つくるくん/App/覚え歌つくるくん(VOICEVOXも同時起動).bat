@echo off

rem VOICEVOXの起動
start "" "%USERPROFILE%\AppData\Local\Programs\VOICEVOX\VOICEVOX.exe"

cd /d "%~dp0"

rem 覚え歌つくるくんの起動
start "" "OboeutaTsukuruKun(Release).exe"
