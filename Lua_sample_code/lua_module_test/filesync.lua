#!/usr/local/bin/lua

require "lfs"
require "md5"

-- Src Directory Info
local gSrcFileListTable = {}
gSrcFileListTable["FileNum"] = 0
gSrcFileListTable["TotalSize"] = 0
local gSrcDirListTable = {}
gSrcDirListTable["DirNum"] = 0
local Src_Path = arg[1] or "src"
-- Dest Directory Info
local gDestFileListTable = {}
gDestFileListTable["FileNum"] = 0
gDestFileListTable["TotalSize"] = 0
local gDestDirListTable = {}
gDestDirListTable["DirNum"] = 0
local Dest_Path = arg[2] or "dest"
--Sync FileList
local gSyncFileListTable = {}

function ListDir (path, FileListTable, DirListTable, TopDir)
	local DirNum, FileNum, md5sum
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path..'/'..file
            local attr = lfs.attributes (f)
            assert (type(attr) == "table")
            if attr.mode == "directory" then
				--DirNum = DirNum + 1
				DirListTable["DirNum"] = DirListTable["DirNum"] + 1
				DirNum = DirListTable["DirNum"]
				DirListTable[DirNum] = {}
				DirListTable[DirNum]["DirName"] = f
                ListDir (f, FileListTable, DirListTable, TopDir)
			else
				--print (f.."\t"..attr["size"].."\t"..FileMD5Cheksum(f))
				--FileNum = FileNum + 1
				--TotalSize = TotalSize + attr["size"]
				FileListTable["FileNum"] = FileListTable["FileNum"] + 1
				FileListTable["TotalSize"] = FileListTable["TotalSize"] + attr["size"]
				FileNum = FileListTable["FileNum"]
				md5sum = FileMD5Cheksum(f)
				if FileListTable[md5sum] == nil then
					--print("md5sumFile:\t"..string.sub(f,string.len(TopDir)+2))
					--print("md5sum:\t"..md5.sumhexa(string.sub(f,string.len(TopDir)+2)))
					--FileListTable[md5sum] = {}
					--FileListTable[md5sum]["FileName"] = f
					--FileListTable[md5sum]["FileSize"] = attr["size"]
					FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))] = {}
					FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))]["FileName"] = f
					FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))]["FileSize"] = attr["size"]
					--FileListTable[FileNum]["md5sum"] = FileMD5Cheksum(f)
				else
					print("md5sum Err:\t"..f)
					--FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))] = {}
					--FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))]["FileName"] = f
					--FileListTable[md5.sumhexa(string.sub(f,string.len(TopDir)+2))]["FileSize"] = attr["size"]
					--FileListTable[FileNum]["md5sum"] = FileMD5Cheksum(f)
				end
			end
        end
    end
end

function FileMD5Cheksum(PathName)
	-- open file                                                           
	FD_tempfile = io.open(PathName,"r")
	FileContext = ""
	FileContext = FD_tempfile:read("*a")
	-- close file
	FD_tempfile:close()
	return md5.sumhexa (FileContext)
end

-- SyncFlag => k: keep, d:delete, a:add
function CompareDir()
	print("CompareDir")	
	local RealSrcPath, RealDestPath
	-- compare md5sum index if dest file is exist?
	for FileIndex in pairs(gDestFileListTable)
	do
		if FileIndex ~= "FileNum" and FileIndex ~= "TotalSize" then
			if gSrcFileListTable[FileIndex] ~= nil then
				RealSrcPath = string.sub(gSrcFileListTable[FileIndex]["FileName"],string.len(Src_Path)+2)
				RealDestPath = string.sub(gDestFileListTable[FileIndex]["FileName"],string.len(Dest_Path)+2)
				-- File in Dest Path
				if RealSrcPath == RealDestPath then
					--print("keep\t"..gDestFileListTable[FileIndex]["FileName"])
					gSyncFileListTable[FileIndex] = {}
					gSyncFileListTable[FileIndex]["FileName"] = gDestFileListTable[FileIndex]["FileName"]
					gSyncFileListTable[FileIndex]["SyncFlag"] = "k"
				else
					-- Dest File is not the same as Src path
					print("del-path name diff src\t"..gSrcFileListTable[FileIndex]["FileName"])
					print("del-path name diff dest\t"..gDestFileListTable[FileIndex]["FileName"])
					gSyncFileListTable[FileIndex] = {}
					gSyncFileListTable[FileIndex]["FileName"] = gDestFileListTable[FileIndex]["FileName"]
					gSyncFileListTable[FileIndex]["SyncFlag"] = "d"
				end
			else
				-- Dest File is not the same as Src path
				print("del-md5sum pathname err\t"..gDestFileListTable[FileIndex]["FileName"])
				gSyncFileListTable[FileIndex] = {}
				gSyncFileListTable[FileIndex]["FileName"] = gDestFileListTable[FileIndex]["FileName"]
				gSyncFileListTable[FileIndex]["SyncFlag"] = "d"
			end
		end
	end

	--- create sync table from src
	for FileIndex in pairs(gSrcFileListTable)
	do
		if FileIndex ~= "FileNum" and FileIndex ~= "TotalSize" then
			if gSyncFileListTable[FileIndex] == nil then
				gSyncFileListTable[FileIndex] = {}
				gSyncFileListTable[FileIndex]["FileName"] = gSrcFileListTable[FileIndex]["FileName"]
				gSyncFileListTable[FileIndex]["SyncFlag"] = "a"
			end
		end
	end

	--ShowSyncFileListTable()
	RemoveDir()
	CreateDestDir()
	CopyFileToDestDir()
end

function ShowSyncFileListTable()
	-- check SyncFileListTable
	for index in pairs(gSyncFileListTable)
	do
		--print(gSyncFileListTable[index]["FileName"].."\t"..gSyncFileListTable[index]["SyncFlag"])
		-- delete file 
		if gSyncFileListTable[index]["SyncFlag"] == "d" then
			print("delete\t"..gSyncFileListTable[index]["FileName"])
		end

		-- copy file
		if gSyncFileListTable[index]["SyncFlag"] == "a" then
			print("copy\t"..gSyncFileListTable[index]["FileName"])
		end
		
		-- keep file
		if gSyncFileListTable[index]["SyncFlag"] == "k" then
			print("keep\t"..gSyncFileListTable[index]["FileName"])
		end
	end
end

function CopyFile(SrcFile,DestFile)
	local FD_SrcFile,FD_DestFile,FileContext
	-- open file                                                           
	FD_SrcFile = io.open(SrcFile,"r")
	FD_DestFile = io.open(DestFile,"w")
	FileContext = ""
	FileContext = FD_SrcFile:read("*a")
	FD_DestFile:write(FileContext)
	-- close file
	FD_SrcFile:close()
	FD_DestFile:close()
end


function CopyFileToDestDir()
	local FilePath,SrcFile,DestFile
	for index in pairs(gSyncFileListTable)
	do
		-- copy file
		if gSyncFileListTable[index]["SyncFlag"] == "a" then
			--readfile
			FilePath = string.sub(gSyncFileListTable[index]["FileName"],string.len(Src_Path)+2)
			SrcFile = gSyncFileListTable[index]["FileName"]
			print("copy\t"..SrcFile)
			DestFile = Dest_Path.."/"..FilePath
			print("to\t"..DestFile)
			CopyFile(SrcFile,DestFile)
		end
		
	end

end

-- algorithm 
-- 1. compare dest and src dir table 
-- 2. list delete dir table
function RemoveDir()
	local FileNamei,SyncFileName,SyncFlag,md5index,attr
	local SrcDirName,DestDirName
	local bKeepDir = false
	
	print("Remove dest dir .....")
	for DirItemNum=gDestDirListTable["DirNum"],0,-1
	do
		for file in lfs.dir(gDestDirListTable[DirItemNum]["DirName"]) 
		do
			if file ~= "." and file ~= ".." then
				FileName = gDestDirListTable[DirItemNum]["DirName"].."/"..file
				attr = lfs.attributes (FileName)
				assert (type(attr) == "table")
				if attr.mode == "file" then
					md5index = md5.sumhexa(string.sub(FileName,string.len(Dest_Path)+2))
					SyncFileName = gSyncFileListTable[md5index]["FileName"]
					SyncFlag = gSyncFileListTable[md5index]["SyncFlag"]
					if SyncFileName == FileName and SyncFlag == "d" then
						print("delete file\t"..SyncFileName)
						os.remove(SyncFileName)
					end
				end
			end
		end
	end
	
	-- remove dest dir if it is not in src dir
	for DestDirItemNum=gDestDirListTable["DirNum"],1,-1
	do
		DestDirName = string.sub(gDestDirListTable[DestDirItemNum]["DirName"],string.len(Dest_Path)+2)
		for  SrcDirItemNum=1,gSrcDirListTable["DirNum"]
		do
			SrcDirName = string.sub(gSrcDirListTable[SrcDirItemNum]["DirName"],string.len(Src_Path)+2)
			if DestDirName == SrcDirName then
				bKeepDir = true
			end
		end

		if bKeepDir == false then
			print("Delete dir with flag\t"..gDestDirListTable[DestDirItemNum]["DirName"])
			os.remove(gDestDirListTable[DestDirItemNum]["DirName"])
		end
		bKeepDir = false
	end
end

function CreateDestDir()
	print("Creating dest dir .....")
	local DirPath,DestDirPath

	for DirItemNum=1,gSrcDirListTable["DirNum"]
	do
		DirPath = string.sub(gSrcDirListTable[DirItemNum]["DirName"],string.len(Src_Path)+2)
		DestDirPath = Dest_Path.."/"..DirPath
		--print("Create\t"..DestDirPath)
		--print(lfs.mkdir(DestDirPath))
		lfs.mkdir(DestDirPath)
	end

end

function ShowFileList(FileListTable)
	-- List gSrcFileListTable 
	print("\nFile name\t\tSize\t\tMd5sum")
	for FileIndex in pairs(FileListTable)
	do
		--print(FileListTable[FileItemNum]["FileName"].."\t\t"..FileListTable[FileItemNum]["filesize"].."\t\t") 
		if FileIndex ~= "FileNum" and FileIndex ~= "TotalSize" then
			print(FileListTable[FileIndex]["FileName"].."\t\t"..FileListTable[FileIndex]["FileSize"].."\t\t"..FileIndex)
		end
	end
end

function ShowDirList(DirListTable, DirNum)
	-- List gSrcDirListTable 
	print("\nDirectory name")
	for DirItemNum=1,DirNum
	do
		print(DirListTable[DirItemNum]["DirName"]) 
	end
end

function FreeMemory()
	-- Free memory
	gSrcFileListTable = nil
	gSrcDirListTable = nil
	gDestFileListTable = nil
	gDestDirListTable = nil
	gSyncFileListTable = nil
	collectgarbage()
end

function CheckArgs()
	local SkipPost
	if #arg < 2 then
		print("Usage: Filesync Src_Dir Dest_Dir")
		os.exit()
	end

	--print("skil arg1 last slash")
	SkipPost = string.sub(arg[1],#arg[1],#arg[1])
	if SkipPost == '/' then
		Src_Path = string.sub(arg[1],0, #arg[1] -1)
	end

	--print("skip arg2 last slash")
	SkipPost = string.sub(arg[2],#arg[2],#arg[2])
	if SkipPost == '/' then
		Dest_Path = string.sub(arg[2],0, #arg[2] -1)
	end

	--arg test
	print("Src_Path: "..Src_Path)
	print("Dest_Path: "..Dest_Path)

	-- check if dest dir is the same as src dir
	if Src_Path == Dest_Path then
		print("Error: Dest dir is the same as Src dit")
		os.exit()
	end

end

-- Main code here
function Main()
	-- check argument and skip '/' in the end of string
	CheckArgs()

	-- put top src top dir in gSrcDirListTable
	gSrcDirListTable[0] = {}
	gSrcDirListTable[0]["DirName"] = Src_Path

	-- Check Src dir
	ListDir(Src_Path,gSrcFileListTable,gSrcDirListTable,Src_Path)
	print("Src DirNum: "..gSrcDirListTable["DirNum"])
	print("Src FileNum: "..gSrcFileListTable["FileNum"])
	print("Src TotalSize: "..gSrcFileListTable["TotalSize"])
	--ShowFileList(gSrcFileListTable)
	--ShowDirList(gSrcDirListTable, gSrcDirListTable["DirNum"])

	-- put top src top dir in gSrcDirListTable
	gDestDirListTable[0] = {}
	gDestDirListTable[0]["DirName"] = Dest_Path

	-- Check Src dir
	ListDir(Dest_Path,gDestFileListTable,gDestDirListTable,Dest_Path)
	print("")
	print("Dest DirNum: "..gDestDirListTable["DirNum"])
	print("Dest FileNum: "..gDestFileListTable["FileNum"])
	print("Dest TotalSize: "..gDestFileListTable["TotalSize"])
	--ShowFileList(gDestFileListTable)
	--ShowDirList(gDestDirListTable, gDestDirListTable["DirNum"])

	CompareDir()

	FreeMemory()


end


-- Main code here
Main()

