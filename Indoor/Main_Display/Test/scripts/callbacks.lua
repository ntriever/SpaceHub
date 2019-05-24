--[[-------- 전역변수 선언 ----------]]--

local curfilepath = ""
local curfilename = ""
local sel
local init = true
-- -- -- -- -- -- -- -- -- --
local RectData = {}
local RectNum = 1
local RectCount = 1
-- -- -- -- -- -- -- -- -- --
local ACData = {}
local ACNum = 1
local ACCount = 1
-- -- -- -- -- -- -- -- -- --
local EmeterData = {}
local EmeterNum = 1
local EmeterCount = 1
-- -- -- -- -- -- -- -- -- --
local TemperData = {}
local TemperNum = 1
local TemperCount = 1
-- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- -- -- --
local EntranceData = {}
local EntranceNum = 1
local EntranceCount = 1
-- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- -- -- --
local ETCData = {}
local ETCNum = 1
local ETCCount = 1
-- -- -- -- -- -- -- -- -- --
--[[----------------------------]]--


--[[-------- 특정 문자열을 기준으로 문자열 자르기 ----------]]--
string.split = function(str, delim, limit)
 local t = {}
 local tt = {}
 local s = str
 local dm = delim
 local limit_t = limit
 local ck = false or limit
 str = nil
 delim = nil
 limit = nil
 while true do
  if s == nil then break end
  local fn = function(t, s, delim)
    local idx = select(2, string.find(s, delim))
    if idx == nil then
     table.insert(t, string.sub(s, 0))
     return nil
    else
     table.insert(t, string.sub(s, 0, idx - 1))
     return string.sub(s, idx + 1)
    end
   end
  s = fn(t, s, dm);
 end
 if not ck then return t end
 if limit_t > table.maxn(t) then print("테이블보다 정한 길이가 큽니다.") return t
 else
  for i = 1, limit_t do
   table.insert(tt, t[i])
  end
  return tt
 end
end
--[[------------------------------------------]]--


--[[-------- 메인화면에서 서브 메뉴선택 ----------]]--
function CBSelectMenu(mapargs)

  local filedata = {}
  
  curfilename = gre.get_value("Main.selected_menu_filename")
  sel = gre.get_value("Main.selected_menu")
  
  filedata = LoadDataFile(mapargs.menu, sel)
  --gre.set_data(filedata)
  
end
--[[------------------------------------]]--

--[[-------- 파일로부터 정보를 읽어와서 처리하는 함수 ----------]]--
function LoadDataFile(fname, sel)

  --curfilepath = gre.SCRIPT_ROOT.."/../datafile/"..fname
  curfilepath = "/mnt/ramdisk/"..fname

  local data = {}
  local linenumber = 1;
  local readdata;

  local read = io.open(curfilepath, "r")

  if(read ~= nil) then  
    for fields in read:lines() do
      if (sel == "emeter") then
        Parsing_Emeter(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "rectifier") then
        Parsing_Rectifier(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "aircon") then
        Parsing_Aircon(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "temperature") then
        Parsing_Temperature(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "entrance") then
        Parsing_Entrance(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "etc") then
        Parsing_ETC(fields,linenumber)
        linenumber = linenumber + 1
        
      elseif(sel == "battery") then
        if(linenumber == 1) then 
          data[sel..".body.text1.text"] = fields
          linenumber = linenumber + 1
          gre.set_data(data)
        end
        --Parsing_Battery(fields,linenumber)
        --linenumber = linenumber + 1
        
      elseif(sel == "setup") then
        Parsing_Setup(fields,linenumber)
        linenumber = linenumber + 1
      end
        
    end
  end  
  

  if (sel == "emeter") then
    EmeterCount = linenumber-1
    UI_Update_Emeter()
  elseif(sel == "rectifier") then
    RectCount = linenumber-1
    UI_Update_Rect()
  elseif(sel == "aircon") then
    ACCount = linenumber-1
    UI_Update_Aircon()
  elseif(sel == "temperature") then
    TemperCount = linenumber-1
    UI_Update_Temperature()
  elseif(sel == "etc") then
    ETCCount = linenumber-1
    UI_Update_ETC()
  elseif(sel == "battery") then
    --RectCount = linenumber-1
  elseif(sel == "entrance") then
    EntranceCount = linenumber-1
    UI_Update_Entrance()
  elseif(sel == "setup") then
    --RectCount = linenumber-1
  end
  --SaveDataFile()
  
  
  return data
  
end
--[[----------------------------------------------]]--


--[[-------- 파일로 정보를 저장하는 함수 *****미사용***** ----------]]--
function SaveDataFile()

  local data = {}
  
 -- data = gre.get_data(sel..".body.TEXT.text")

--  local n = data[sel..".body.TEXT.text"] .. "\n"
  

  local test = io.open(curfilepath, "w")
  if(test ~= nil) then
   -- test:write(n)
    test:write("TESTTEXT/TESTTEXT/TESTTEXT/1\n")
    test:write("TESTTEXT/TESTTEXT/TESTTEXT/2\n")
    test:write("TESTTEXT/TESTTEXT/TESTTEXT/3\n")
  end  
  
  test:close()
  
  curfilepath = ""
  
end
--[[---------------------------------------------------]]--


--[[-------- 보드 실행 시 초기화 함수----------]]--
function Init(mapargs) 

  if(init == true) then
    local data = {}    
    --local fipaddress = io.open(gre.SCRIPT_ROOT.."/../datafile/ip.ntx","r")
    local fimain = io.open("/mnt/ramdisk/main.ntx","r")
  
    --[[-- 펌웨어 버전 / 기지국 아이디 / 로컬 아이피  데이터 읽어오기 --]]--
    if(fimain ~= nil) then  
      for fields in fimain:lines() do
        readdata = string.split(fields, ",")
        for l, v in pairs(readdata) do
          if(l==1) then
            data["Main.titlebar.version.text"] = v
          elseif(l==2) then
            data["Main.titlebar.id.text"]  = v
          elseif(l==3) then
           data["Main.titlebar.ip.text"]  = v
          end
          gre.set_data(data)
        end
      end
    end
    --[[----------------------------------]]--
  end
end
--[[----------------------------------]]--

--[[-------- 서브메뉴에서 back 버튼 누를 경우----------]]--
function Action_BtnBack(mapargs)
  
  local data = {}
  --local fipaddress = io.open(gre.SCRIPT_ROOT.."/../datafile/ip.ntx","r")
  local fimain = io.open("/mnt/ramdisk/main.ntx","r")
  
  if(fimain ~= nil) then  
    for fields in fimain:lines() do
      readdata = string.split(fields, ",")
      for l, v in pairs(readdata) do
        if(l==1) then
          data["Main.titlebar.version.text"] = v
        elseif(l==2) then
          data["Main.titlebar.id.text"]  = v
        elseif(l==3) then
         data["Main.titlebar.ip.text"]  = v
--[[     elseif(l==4) then
         data["Main.titlebar.com.comstate"]  = tonumber(v)
         elseif(l==5) then
         data["Main.titlebar.alm.almstate"]  = tonumber(v) ]]--
        end
        gre.set_data(data)
      end
    end
  end
end
--[[-----------------------------------------]]--

--[[-------- 서브메뉴 화면에서 정보를 1초마다 갱신----------]]--
function UpdateInfo(mapargs) 

  local filedata = {}

  filedata = LoadDataFile(curfilename, sel)
  --gre.set_data(filedata)

end
--[[------------------------------------------]]--

--[[--------정류기 정보 파싱----------]]--
function Parsing_Rectifier(info,line)

  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
    if(l==1) then
      RectData[line.."_"..l] = tonumber(v)
    end
    
    if(RectData[line.."_1"] == 0) then
      if(l==2) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==3) then
        RectData[line.."_"..l] = v
      elseif(l==4) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==5) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==6) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==7) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==8) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==9) then
        RectData[line.."_"..l] = v
      elseif(l==10) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==11) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==12) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==13) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==14) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==15) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==16) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==17) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==18) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==19) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==20) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==21) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==22) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==23) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==24) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==25) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==26) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==27) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==28) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==29) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==30) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==31) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==32) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==33) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==34) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==35) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==36) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==37) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==38) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==39) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==40) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==41) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==42) then
        RectData[line.."_"..l] = tonumber(v)
      elseif(l==43) then
        RectData[line.."_"..l] = tonumber(v)
      end
    else if(RectData[line.."_1"] == 1) then
        if(l==2) then
          RectData[line.."_"..l] = tonumber(v)
        elseif(l==3) then
          RectData[line.."_"..l] = tonumber(v)
        elseif(l==4) then
          RectData[line.."_"..l] = tonumber(v)
        end
        end
    end
    gre.set_data(RectData)
    end
end
--[[---------------------------]]--

--[[--------정류기 UI update----------]]--
function UI_Update_Rect()
  local data = {}
  local body = {}
  local body2 = {}
  
  
    
  if (RectNum >= RectCount) then
    RectNum = RectCount
    data[sel..".body.arrowR.grd_x"] = 480
  else if (RectNum <= 1) then
    RectNum = 1
    data[sel..".body.arrowL.grd_x"] = -70
  end
  end
  
  if(RectData[RectNum.."_1"] == 0) then
    body["hidden"] = 0
    body2["hidden"] = 1
  else if(RectData[RectNum.."_1"] == 1) then
    body["hidden"] = 1
    body2["hidden"] = 0
  end
  end

  gre.set_group_attrs("rectifier.body",body)
  gre.set_group_attrs("rectifier.body2",body2)
  
  if(RectData[RectNum.."_1"] == 0) then
    
    if(RectData[RectNum.."_2"] == nil) then
      RectData[RectNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = RectData[RectNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = RectData[RectNum.."_3"]
        
    if(RectData[RectNum.."_4"] == 0) then
      data[sel..".body.com.comcolor"] =  16777215
    else
      data[sel..".body.com.comcolor"] =  65280
    end
    
    if(RectData[RectNum.."_5"] == 0) then
      data[sel..".body.ov.comcolor"] =  16777215
    else
      data[sel..".body.ov.comcolor"] =  16711680
    end
    
    if(RectData[RectNum.."_6"] == 0) then
      data[sel..".body.uv.comcolor"] =  16777215
    else
      data[sel..".body.uv.comcolor"] =  16711680
    end
    
    if(RectData[RectNum.."_7"] == 0) then
      data[sel..".body.etcalm.comcolor"] =  16777215
    else
      data[sel..".body.etcalm.comcolor"] =  16711680
    end
    
    if(RectData[RectNum.."_8"] == nil) then
      RectData[RectNum.."_8"] = 0
    else
      data[sel..".body.ACvoltage.text"] = RectData[RectNum.."_8"].." [V]"
    end
    
    if(RectData[RectNum.."_9"] == nil) then
      RectData[RectNum.."_9"] = "OFF"
    else
      data[sel..".body.chargemode.text"] = RectData[RectNum.."_9"]
    end
    
    if(RectData[RectNum.."_10"] == nil) then
      RectData[RectNum.."_10"] = 0
    else
      data[sel..".body.outvoltage.text"] = RectData[RectNum.."_10"].." [V]"
    end
    
    if(RectData[RectNum.."_11"] == nil) then
      RectData[RectNum.."_11"] = 0
    else
      data[sel..".body.outcurrent.text"] = RectData[RectNum.."_11"].." [A]"
    end
    
    if(RectData[RectNum.."_12"] == nil) then
      RectData[RectNum.."_12"] = 0
    else
      data[sel..".body.batteryvoltage.text"] = RectData[RectNum.."_12"].." [V]"
    end
    
    if(RectData[RectNum.."_13"] == nil) then
      RectData[RectNum.."_13"] = 0
    else
      data[sel..".body.modulecurrent.text"] = RectData[RectNum.."_13"].." [A]"
    end
    
    if(RectData[RectNum.."_14"] == 0) then
      data[sel..".body.rectifier1.color"] =  16777215
    else
      data[sel..".body.rectifier1.color"] =  16711680
    end
    
    if(RectData[RectNum.."_15"] == 0) then
      data[sel..".body.rectifier2.color"] =  16777215
    else
      data[sel..".body.rectifier2.color"] =  16711680
    end
    
    if(RectData[RectNum.."_16"] == 0) then
      data[sel..".body.rectifier3.color"] =  16777215
    else
      data[sel..".body.rectifier3.color"] =  16711680
    end
    
    if(RectData[RectNum.."_17"] == 0) then
      data[sel..".body.rectifier4.color"] =  16777215
    else
      data[sel..".body.rectifier4.color"] =  16711680
    end
    
    if(RectData[RectNum.."_18"] == 0) then
      data[sel..".body.rectifier5.color"] =  16777215
    else
      data[sel..".body.rectifier5.color"] =  16711680
    end
    
    if(RectData[RectNum.."_19"] == 0) then
      data[sel..".body.rectifier6.color"] =  16777215
    else
      data[sel..".body.rectifier6.color"] =  16711680
    end
    
    if(RectData[RectNum.."_20"] == 0) then
      data[sel..".body.rectifier7.color"] =  16777215
    else
      data[sel..".body.rectifier7.color"] =  16711680
    end
    
    if(RectData[RectNum.."_21"] == 0) then
      data[sel..".body.rectifier8.color"] =  16777215
    else
      data[sel..".body.rectifier8.color"] =  16711680
    end
    
    if(RectData[RectNum.."_22"] == 0) then
      data[sel..".body.rectifier9.color"] =  16777215
    else
      data[sel..".body.rectifier9.color"] =  16711680
    end
    
    if(RectData[RectNum.."_23"] == 0) then
      data[sel..".body.rectifier10.color"] =  16777215
    else
      data[sel..".body.rectifier10.color"] =  16711680
    end
    
    if(RectData[RectNum.."_24"] == 0) then
      data[sel..".body.rectifier11.color"] =  16777215
    else
      data[sel..".body.rectifier11.color"] =  16711680
    end
    
    if(RectData[RectNum.."_25"] == 0) then
      data[sel..".body.rectifier12.color"] =  16777215
    else
      data[sel..".body.rectifier12.color"] =  16711680
    end
    
    if(RectData[RectNum.."_26"] == 0) then
      data[sel..".body.rectifier13.color"] =  16777215
    else
      data[sel..".body.rectifier13.color"] =  16711680
    end
    
    if(RectData[RectNum.."_27"] == 0) then
      data[sel..".body.rectifier14.color"] =  16777215
    else
      data[sel..".body.rectifier14.color"] =  16711680
    end
    
    if(RectData[RectNum.."_28"] == 0) then
      data[sel..".body.rectifier15.color"] =  16777215
    else
      data[sel..".body.rectifier15.color"] =  16711680
    end
    
    if(RectData[RectNum.."_29"] == 0) then
      data[sel..".body.rectifier16.color"] =  16777215
    else
      data[sel..".body.rectifier16.color"] =  16711680
    end
    
    if(RectData[RectNum.."_30"] == 0) then
      data[sel..".body.rectifier17.color"] =  16777215
    else
      data[sel..".body.rectifier17.color"] =  16711680
    end
    
    if(RectData[RectNum.."_31"] == 0) then
      data[sel..".body.rectifier18.color"] =  16777215
    else
      data[sel..".body.rectifier18.color"] =  16711680
    end
    
    if(RectData[RectNum.."_32"] == 0) then
      data[sel..".body.rectifier19.color"] =  16777215
    else
      data[sel..".body.rectifier19.color"] =  16711680
    end
    
    if(RectData[RectNum.."_33"] == 0) then
      data[sel..".body.rectifier20.color"] =  16777215
    else
      data[sel..".body.rectifier20.color"] =  16711680
    end
    
    if(RectData[RectNum.."_34"] == 0) then
      data[sel..".body.rectifier21.color"] =  16777215
    else
      data[sel..".body.rectifier21.color"] =  16711680
    end
    
    if(RectData[RectNum.."_35"] == 0) then
      data[sel..".body.rectifier22.color"] =  16777215
    else
      data[sel..".body.rectifier22.color"] =  16711680
    end
    
    if(RectData[RectNum.."_36"] == 0) then
      data[sel..".body.rectifier23.color"] =  16777215
    else
      data[sel..".body.rectifier23.color"] =  16711680
    end
    
    if(RectData[RectNum.."_37"] == 0) then
      data[sel..".body.rectifier24.color"] =  16777215
    else
      data[sel..".body.rectifier24.color"] =  16711680
    end
    
    if(RectData[RectNum.."_38"] == 0) then
      data[sel..".body.rectifier25.color"] =  16777215
    else
      data[sel..".body.rectifier25.color"] =  16711680
    end
    
    if(RectData[RectNum.."_39"] == 0) then
      data[sel..".body.rectifier26.color"] =  16777215
    else
      data[sel..".body.rectifier26.color"] =  16711680
    end
    
    if(RectData[RectNum.."_40"] == 0) then
      data[sel..".body.rectifier27.color"] =  16777215
    else
      data[sel..".body.rectifier27.color"] =  16711680
    end
    
    if(RectData[RectNum.."_41"] == 0) then
      data[sel..".body.rectifier28.color"] =  16777215
    else
      data[sel..".body.rectifier28.color"] =  16711680
    end
    
    if(RectData[RectNum.."_42"] == 0) then
      data[sel..".body.rectifier29.color"] =  16777215
    else
      data[sel..".body.rectifier29.color"] =  16711680
    end
    
    if(RectData[RectNum.."_43"] == 0) then
      data[sel..".body.rectifier30.color"] =  16777215
    else
      data[sel..".body.rectifier30.color"] =  16711680
    end

  else if(RectData[RectNum.."_1"] == 1) then
  
    if(RectData[RectNum.."_2"] == nil) then
      RectData[RectNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = RectData[RectNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "정류기 감시 모듈"
        
    if(RectData[RectNum.."_3"] == 0) then
      data[sel..".body2.com.comcolor"] =  16777215
    else
      data[sel..".body2.com.comcolor"] =  65280
    end
    
    if(RectData[RectNum.."_4"] == nil) then
      RectData[RectNum.."_4"] = 0
    else
      data[sel..".body2.voltage.text"] = RectData[RectNum.."_4"].." [V]"
    end
    
  end
  end
    gre.set_data(data)
end
--[[---------------------------]]--

--[[--------냉방기 정보 파싱----------]]--
function Parsing_Aircon(info,line)
  
  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
    if(l==1) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==2) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==3) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==4) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==5) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==6) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==7) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==8) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==9) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==10) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==11) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==12) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==13) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==14) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==15) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==16) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==17) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==18) then
      ACData[line.."_"..l] = tonumber(v)
    elseif(l==19) then
      ACData[line.."_"..l] = tonumber(v)
    end
    gre.set_data(ACData)
    end
end
--[[---------------------------]]--

--[[--------냉방기 UI update----------]]--
function UI_Update_Aircon()
  
  local data = {}
  local body = {}
  local body2 = {}
  local body3 = {}
  local body4 = {}
  local body5 = {}
  
  if (ACNum >= ACCount) then
    ACNum = ACCount
    data[sel..".arrow.arrowR.grd_x"] = 480
  else if (ACNum <= 1) then
    ACNum = 1
    data[sel..".arrow.arrowL.grd_x"] = -70
  end
  end
  
  if(ACData[ACNum.."_1"] == 0) then
    body["hidden"] = 0
    body2["hidden"] = 1
    body3["hidden"] = 1
    body4["hidden"] = 1
    body5["hidden"] = 1
  else if(ACData[ACNum.."_1"] == 1) then
    body["hidden"] = 1
    body2["hidden"] = 0
    body3["hidden"] = 1
    body4["hidden"] = 1
    body5["hidden"] = 1
  else if(ACData[ACNum.."_1"] == 2) then
    body["hidden"] = 1
    body2["hidden"] = 1
    body3["hidden"] = 0
    body4["hidden"] = 1
    body5["hidden"] = 1
  else if(ACData[ACNum.."_1"] == 3) then
    body["hidden"] = 1
    body2["hidden"] = 1
    body3["hidden"] = 1
    body4["hidden"] = 0
    body5["hidden"] = 1
  else if(ACData[ACNum.."_1"] == 4) then
    body["hidden"] = 1
    body2["hidden"] = 1
    body3["hidden"] = 1
    body4["hidden"] = 1
    body5["hidden"] = 0
  end
  end
  end
  end
  end
  
  gre.set_group_attrs("aircon.body",body)
  gre.set_group_attrs("aircon.body2",body2)
  gre.set_group_attrs("aircon.body3",body3)
  gre.set_group_attrs("aircon.body4",body4)
  gre.set_group_attrs("aircon.body5",body5)
  
  --[[-- 가변용량 냉방기 --]]--
  if(ACData[ACNum.."_1"] == 0) then
  
    if(ACData[ACNum.."_2"] == nil) then
      ACData[ACNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ACData[ACNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "가변용량 냉방기"
        
    if(ACData[ACNum.."_3"] == 0) then
      data[sel..".body.com.comcolor"] =  16777215
    else
      data[sel..".body.com.comcolor"] =  65280
    end
    
    if(ACData[ACNum.."_4"] == nil) then
      ACData[ACNum.."_4"] = 0
    else
      if(ACData[ACNum.."_4"] == 1) then
       data[sel..".body.onoff.text"] = "RUN"
     else
       data[sel..".body.onoff.text"] = "STOP"
     end
    end
    
    if(ACData[ACNum.."_5"] == nil) then
      ACData[ACNum.."_5"] = 0
    else
      data[sel..".body.currentconsumption.text"] = ACData[ACNum.."_5"].." [A]"
    end
    
    if(ACData[ACNum.."_6"] == nil) then
      ACData[ACNum.."_6"] = 0
    else
      data[sel..".body.indoortemp.text"] = ACData[ACNum.."_6"].." [℃]"
    end
    
    if(ACData[ACNum.."_7"] == nil) then
      ACData[ACNum.."_7"] = 0
    else
      data[sel..".body.dischargetemp.text"] = ACData[ACNum.."_7"].." [℃]"
    end
    
    if(ACData[ACNum.."_8"] == nil) then
      ACData[ACNum.."_8"] = 0
    else
      data[sel..".body.indoorhum.text"] = ACData[ACNum.."_8"].." [%]"
    end
    
    if(ACData[ACNum.."_9"] == nil) then
      ACData[ACNum.."_9"] = 0
    else
      data[sel..".body.dischargehum.text"] = ACData[ACNum.."_9"].." [%]"
    end
    
    if(ACData[ACNum.."_10"] == 0) then
      data[sel..".body.COMP.color"] =  16777215
    else
      data[sel..".body.COMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_11"] == 0) then
      data[sel..".body.highpressure.color"] =  16777215
    else
      data[sel..".body.highpressure.color"] =  16711680
    end
    
    if(ACData[ACNum.."_12"] == 0) then
      data[sel..".body.FAN.color"] =  16777215
    else
      data[sel..".body.FAN.color"] =  16711680
    end
    
    if(ACData[ACNum.."_13"] == 0) then
      data[sel..".body.lowpressure.color"] =  16777215
    else
      data[sel..".body.lowpressure.color"] =  16711680
    end
    
    if(ACData[ACNum.."_14"] == 0) then
      data[sel..".body.sensorerror.color"] =  16777215
    else
      data[sel..".body.sensorerror.color"] =  16711680
    end
  --[[---------------]]--
  
  --[[-- 자연공조 냉방기 --]]--
  else if(ACData[ACNum.."_1"] == 1) then
  
    if(ACData[ACNum.."_2"] == nil) then
      ACData[ACNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ACData[ACNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "자연공조 냉방기"
        
    if(ACData[ACNum.."_3"] == 0) then
      data[sel..".body2.com.comcolor"] =  16777215
    else
      data[sel..".body2.com.comcolor"] =  65280
    end
    
    if(ACData[ACNum.."_4"] == nil) then
      ACData[ACNum.."_4"] = 0
    else
      data[sel..".body2.indoortemp.text"] = ACData[ACNum.."_4"].." [℃]"
    end
    
    if(ACData[ACNum.."_5"] == nil) then
      ACData[ACNum.."_5"] = 0
    else
      data[sel..".body2.watertemp.text"] = ACData[ACNum.."_5"].." [℃]"
    end
    
    if(ACData[ACNum.."_6"] == nil) then
      ACData[ACNum.."_6"] = 0
    else
      data[sel..".body2.outdoortemp.text"] = ACData[ACNum.."_6"].." [℃]"
    end
    
    if(ACData[ACNum.."_7"] == nil) then
      ACData[ACNum.."_7"] = 0
    else
      data[sel..".body2.frozentemp.text"] = ACData[ACNum.."_7"].." [℃]"
    end
    
    if(ACData[ACNum.."_8"] == 0) then
      data[sel..".body2.COMP.color"] =  16777215
    else
      data[sel..".body2.COMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_9"] == 0) then
      data[sel..".body2.FAN.color"] =  16777215
    else
      data[sel..".body2.FAN.color"] =  16711680
    end
    
    if(ACData[ACNum.."_10"] == 0) then
      data[sel..".body2.PUMP.color"] =  16777215
    else
      data[sel..".body2.PUMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_11"] == 0) then
      data[sel..".body2.fluid.color"] =  16777215
    else
      data[sel..".body2.fluid.color"] =  16711680
    end
    
    if(ACData[ACNum.."_12"] == 0) then
      data[sel..".body2.cumulative.color"] =  16777215
    else
      data[sel..".body2.cumulative.color"] =  16711680
    end
    
    if(ACData[ACNum.."_13"] == 0) then
      data[sel..".body2.lowtemp.color"] =  16777215
    else
      data[sel..".body2.lowtemp.color"] =  16711680
    end
    
    if(ACData[ACNum.."_14"] == 0) then
      data[sel..".body2.indoortempsensor.color"] =  16777215
    else
      data[sel..".body2.indoortempsensor.color"] =  16711680
    end
    
    if(ACData[ACNum.."_15"] == 0) then
      data[sel..".body2.outdoortempsensor.color"] =  16777215
    else
      data[sel..".body2.outdoortempsensor.color"] =  16711680
    end
    
    if(ACData[ACNum.."_16"] == 0) then
      data[sel..".body2.watertempsensor.color"] =  16777215
    else
      data[sel..".body2.watertempsensor.color"] =  16711680
    end
    
    if(ACData[ACNum.."_17"] == 0) then
      data[sel..".body2.frozentempsensor.color"] =  16777215
    else
      data[sel..".body2.frozentempsensor.color"] =  16711680
    end
    
    if(ACData[ACNum.."_18"] == 0) then
      data[sel..".body2.drain.color"] =  16777215
    else
      data[sel..".body2.drain.color"] =  16711680
    end
    
    if(ACData[ACNum.."_19"] == 0) then
      data[sel..".body2.watertank.color"] =  16777215
    else
      data[sel..".body2.watertank.color"] =  16711680
    end
  --[[---------------]]--
  
  --[[-- 창조1 냉방기 --]]--
  else if(ACData[ACNum.."_1"] == 2) then
  
  if(ACData[ACNum.."_2"] == nil) then
      ACData[ACNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ACData[ACNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "창조1 냉방기"
        
    if(ACData[ACNum.."_3"] == 0) then
      data[sel..".body3.com.comcolor"] =  16777215
    else
      data[sel..".body3.com.comcolor"] =  65280
    end
    
    if(ACData[ACNum.."_4"] == 0) then
      data[sel..".body3.COMP.color"] =  16777215
    else
      data[sel..".body3.COMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_5"] == 0) then
      data[sel..".body3.FAN.color"] =  16777215
    else
      data[sel..".body3.FAN.color"] =  16711680
    end
  --[[---------------]]--
  
  --[[-- 창조2 냉방기 --]]--
  else if(ACData[ACNum.."_1"] == 3) then
    if(ACData[ACNum.."_2"] == nil) then
      ACData[ACNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ACData[ACNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "자연공조 냉방기"
        
    if(ACData[ACNum.."_3"] == 0) then
      data[sel..".body4.com.comcolor"] =  16777215
    else
      data[sel..".body4.com.comcolor"] =  65280
    end
    
    if(ACData[ACNum.."_4"] == nil) then
      ACData[ACNum.."_4"] = 0
    else
      if(ACData[ACNum.."_4"] == 1) then
       data[sel..".body4.onoff.text"] = "RUN"
     else
       data[sel..".body4.onoff.text"] = "STOP"
     end
    end
    
    if(ACData[ACNum.."_5"] == nil) then
      ACData[ACNum.."_5"] = 0
    else
      data[sel..".body4.indoortemp.text"] = ACData[ACNum.."_5"].." [℃]"
    end
    
    if(ACData[ACNum.."_6"] == nil) then
      ACData[ACNum.."_6"] = 0
    else
      data[sel..".body4.watertemp.text"] = ACData[ACNum.."_6"].." [℃]"
    end
    
    if(ACData[ACNum.."_7"] == nil) then
      ACData[ACNum.."_7"] = 0
    else
      data[sel..".body4.outdoortemp.text"] = ACData[ACNum.."_7"].." [℃]"
    end
    
    if(ACData[ACNum.."_8"] == nil) then
      ACData[ACNum.."_8"] = 0
    else
      data[sel..".body4.frozentemp.text"] = ACData[ACNum.."_8"].." [℃]"
    end
    
    if(ACData[ACNum.."_9"] == 0) then
      data[sel..".body4.COMP.color"] =  16777215
    else
      data[sel..".body4.COMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_10"] == 0) then
      data[sel..".body4.PUMP.color"] =  16777215
    else
      data[sel..".body4.PUMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_11"] == 0) then
      data[sel..".body4.iFAN.color"] =  16777215
    else
      data[sel..".body4.iFAN.color"] =  16711680
    end
    
    if(ACData[ACNum.."_12"] == 0) then
      data[sel..".body4.fluid.color"] =  16777215
    else
      data[sel..".body4.fluid.color"] =  16711680
    end
    
    if(ACData[ACNum.."_13"] == 0) then
      data[sel..".body4.cumulative.color"] =  16777215
    else
      data[sel..".body4.cumulative.color"] =  16711680
    end
    
    if(ACData[ACNum.."_14"] == 0) then
      data[sel..".body4.oFAN.color"] =  16777215
    else
      data[sel..".body4.oFAN.color"] =  16711680
    end
    
    if(ACData[ACNum.."_15"] == 0) then
      data[sel..".body4.hightemp.color"] =  16777215
    else
      data[sel..".body4.hightemp.color"] =  16711680
    end
    
    if(ACData[ACNum.."_16"] == 0) then
      data[sel..".body4.lowtemp.color"] =  16777215
    else
      data[sel..".body4.lowtemp.color"] =  16711680
    end
    
    if(ACData[ACNum.."_17"] == 0) then
      data[sel..".body4.frozen.color"] =  16777215
    else
      data[sel..".body4.frozen.color"] =  16711680
    end
    
    if(ACData[ACNum.."_18"] == 0) then
      data[sel..".body4.tempsensor.color"] =  16777215
    else
      data[sel..".body4.tempsensor.color"] =  16711680
    end
  --[[---------------]]--
  
  --[[-- 온도 감시 모듈 --]]--
  else if(ACData[ACNum.."_1"] == 4) then
  
    if(ACData[ACNum.."_2"] == nil) then
      ACData[ACNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ACData[ACNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "냉방기 김시 모듈"
        
    if(ACData[ACNum.."_3"] == 0) then
      data[sel..".body5.com.comcolor"] =  16777215
    else
      data[sel..".body5.com.comcolor"] =  65280
    end
    
    if(ACData[ACNum.."_4"] == 0) then
      data[sel..".body5.AC.color"] =  16777215
    else
      data[sel..".body5.AC.color"] =  16711680
    end
    
    if(ACData[ACNum.."_5"] == 0) then
      data[sel..".body5.COMP.color"] =  16777215
    else
      data[sel..".body5.COMP.color"] =  16711680
    end
    
    if(ACData[ACNum.."_6"] == 0) then
      data[sel..".body5.FAN.color"] =  16777215
    else
      data[sel..".body5.FAN.color"] =  16711680
    end
  --[[---------------]]--
  end
  end
  end
  end
  end

  gre.set_data(data)
end
--[[---------------------------]]--

--[[--------전력량 정보 파싱----------]]--
function Parsing_Emeter(info,line)
  
  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
    if(l==1) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==2) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==3) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==4) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==5) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==6) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==7) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==8) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==9) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==10) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==11) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==12) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==13) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==14) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==15) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==16) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==17) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==18) then
      EmeterData[line.."_"..l] = tonumber(v)
    elseif(l==19) then
      EmeterData[line.."_"..l] = tonumber(v)
    end
    gre.set_data(EmeterData)
    end
end
--[[---------------------------]]--

--[[--------전력량 UI update----------]]--
function UI_Update_Emeter()
  local data = {}
  local body = {}
  local body2 = {}
  local body3 = {}
  
  if (EmeterNum >= EmeterCount) then
    EmeterNum = EmeterCount
    data[sel..".arrow.arrowR.grd_x"] = 480
  else if (EmeterNum <= 1) then
    EmeterNum = 1
    data[sel..".arrow.arrowL.grd_x"] = -70
  end
  end
  
  if(EmeterData[EmeterNum.."_1"] == 0) then
    body["hidden"] = 0
    body2["hidden"] = 1
    body3["hidden"] = 1
  else if(EmeterData[EmeterNum.."_1"] == 1) then
    body["hidden"] = 1
    body2["hidden"] = 0
    body3["hidden"] = 1
  else if(EmeterData[EmeterNum.."_1"] == 2) then
    body["hidden"] = 1
    body2["hidden"] = 1
    body3["hidden"] = 0
  end
  end
  end
  
  gre.set_group_attrs("emeter.body",body)
  gre.set_group_attrs("emeter.body2",body2)
  gre.set_group_attrs("emeter.body3",body3)
  
  --[[-- 전력량계 --]]--
  if(EmeterData[EmeterNum.."_1"] == 0) then
    
    if(EmeterData[EmeterNum.."_2"] == nil) then
      EmeterData[EmeterNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = EmeterData[EmeterNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "다채널 전력량계"
        
    if(EmeterData[EmeterNum.."_3"] == 0) then
      data[sel..".body.com.comcolor"] =  16777215
    else
      data[sel..".body.com.comcolor"] =  65280
    end
    
    if(EmeterData[EmeterNum.."_4"] == nil) then
      EmeterData[EmeterNum.."_4"] = 0
    else
      data[sel..".body.channelnum.text"] = EmeterData[EmeterNum.."_4"]
    end
    
    if(EmeterData[EmeterNum.."_5"] == nil) then
      EmeterData[EmeterNum.."_5"] = 0
    else
      data[sel..".body.totalpower.text"] = EmeterData[EmeterNum.."_5"].." [W]"
    end
    
    if(EmeterData[EmeterNum.."_6"] == nil) then
      EmeterData[EmeterNum.."_6"] = 0
    else
      data[sel..".body.avgPvoltage.text"] = EmeterData[EmeterNum.."_6"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_7"] == nil) then
      EmeterData[EmeterNum.."_7"] = 0
    else
      data[sel..".body.avglinevoltage.text"] = EmeterData[EmeterNum.."_7"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_8"] == nil) then
      EmeterData[EmeterNum.."_8"] = 0
    else
      data[sel..".body.RPvoltage.text"] = EmeterData[EmeterNum.."_8"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_9"] == nil) then
      EmeterData[EmeterNum.."_9"] = 0
    else
      data[sel..".body.RSlinevoltage.text"] = EmeterData[EmeterNum.."_9"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_10"] == nil) then
      EmeterData[EmeterNum.."_10"] = 0
    else
      data[sel..".body.SPvoltage.text"] = EmeterData[EmeterNum.."_10"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_11"] == nil) then
      EmeterData[EmeterNum.."_11"] = 0
    else
      data[sel..".body.STlinevoltage.text"] = EmeterData[EmeterNum.."_11"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_12"] == nil) then
      EmeterData[EmeterNum.."_12"] = 0
    else
      data[sel..".body.TPvoltage.text"] = EmeterData[EmeterNum.."_12"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_13"] == nil) then
      EmeterData[EmeterNum.."_13"] = 0
    else
      data[sel..".body.TRlinevoltage.text"] = EmeterData[EmeterNum.."_13"].." [V]"
    end
    
    if(EmeterData[EmeterNum.."_14"] == nil) then
      EmeterData[EmeterNum.."_14"] = 0
    else
      data[sel..".body.RPcurrent.text"] = EmeterData[EmeterNum.."_14"].." [A]"
    end
    
    if(EmeterData[EmeterNum.."_15"] == nil) then
      EmeterData[EmeterNum.."_15"] = 0
    else
      data[sel..".body.RPpower.text"] = EmeterData[EmeterNum.."_15"].." [W]"
    end
    
    if(EmeterData[EmeterNum.."_16"] == nil) then
      EmeterData[EmeterNum.."_16"] = 0
    else
      data[sel..".body.SPcurrent.text"] = EmeterData[EmeterNum.."_16"].." [A]"
    end
    
    if(EmeterData[EmeterNum.."_17"] == nil) then
      EmeterData[EmeterNum.."_17"] = 0
    else
      data[sel..".body.SPpower.text"] = EmeterData[EmeterNum.."_17"].." [W]"
    end
    
    if(EmeterData[EmeterNum.."_18"] == nil) then
      EmeterData[EmeterNum.."_18"] = 0
    else
      data[sel..".body.TPcurrent.text"] = EmeterData[EmeterNum.."_18"].." [A]"
    end
 
    if(EmeterData[EmeterNum.."_19"] == nil) then
      EmeterData[EmeterNum.."_19"] = 0
    else
      data[sel..".body.TPpower.text"] = EmeterData[EmeterNum.."_19"].." [W]"
    end
  --[[---------------]]--
  
  --[[-- 전력량계/채널 --]]--
  else if(EmeterData[EmeterNum.."_1"] == 1) then
  
    if(EmeterData[EmeterNum.."_2"] == nil) then
      EmeterData[EmeterNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = EmeterData[EmeterNum.."_2"]
    end
    
    data[sel..".titlebar.devicename.text"] = "다채널 전력량계"
        
    if(EmeterData[EmeterNum.."_3"] == 0) then
      data[sel..".body2.com.comcolor"] =  16777215
    else
      data[sel..".body2.com.comcolor"] =  65280
    end
    
    if(EmeterData[EmeterNum.."_4"] == nil) then
      EmeterData[EmeterNum.."_4"] = 0
    else
      data[sel..".body2.channelnum.text"] = EmeterData[EmeterNum.."_4"]
    end
    
    if(EmeterData[EmeterNum.."_5"] == nil) then
      EmeterData[EmeterNum.."_5"] = 0
    else
      data[sel..".body2.RPcurrent.text"] = EmeterData[EmeterNum.."_5"]
    end
    
    if(EmeterData[EmeterNum.."_6"] == nil) then
      EmeterData[EmeterNum.."_6"] = 0
    else
      data[sel..".body2.RPpowerfactor.text"] = EmeterData[EmeterNum.."_6"]
    end
    
    if(EmeterData[EmeterNum.."_7"] == nil) then
      EmeterData[EmeterNum.."_7"] = 0
    else
      data[sel..".body2.SPcurrent.text"] = EmeterData[EmeterNum.."_7"]
    end
    
    if(EmeterData[EmeterNum.."_8"] == nil) then
      EmeterData[EmeterNum.."_8"] = 0
    else
      data[sel..".body2.SPpowerfactor.text"] = EmeterData[EmeterNum.."_8"]
    end
    
    if(EmeterData[EmeterNum.."_9"] == nil) then
      EmeterData[EmeterNum.."_9"] = 0
    else
      data[sel..".body2.TPcurrent.text"] = EmeterData[EmeterNum.."_9"]
    end
    
    if(EmeterData[EmeterNum.."_10"] == nil) then
      EmeterData[EmeterNum.."_10"] = 0
    else
      data[sel..".body2.TPpowerfactor.text"] = EmeterData[EmeterNum.."_10"]
    end
    
    if(EmeterData[EmeterNum.."_11"] == nil) then
      EmeterData[EmeterNum.."_11"] = 0
    else
      data[sel..".body2.totoalpower.text"] = EmeterData[EmeterNum.."_11"]
    end
    
    if(EmeterData[EmeterNum.."_12"] == nil) then
      EmeterData[EmeterNum.."_12"] = 0
    else
      data[sel..".body2.RPpower.text"] = EmeterData[EmeterNum.."_12"]
    end
    
    if(EmeterData[EmeterNum.."_13"] == nil) then
      EmeterData[EmeterNum.."_13"] = 0
    else
      data[sel..".body2.SPpower.text"] = EmeterData[EmeterNum.."_13"]
    end
    
    if(EmeterData[EmeterNum.."_14"] == nil) then
      EmeterData[EmeterNum.."_14"] = 0
    else
      data[sel..".body2.TPpower.text"] = EmeterData[EmeterNum.."_14"]
    end
    --[[-----------------]]--
    
    --[[-- 상전감시모듈 --]]--
    else if(EmeterData[EmeterNum.."_1"] == 2) then
  
      if(EmeterData[EmeterNum.."_2"] == nil) then
        EmeterData[EmeterNum.."_2"] = 0
      else
        data[sel..".titlebar.devicenum.text"] = EmeterData[EmeterNum.."_2"]
      end
      
      data[sel..".titlebar.devicename.text"] = "상전 감시 모듈"
          
      if(EmeterData[EmeterNum.."_3"] == 0) then
        data[sel..".body3.com.comcolor"] =  16777215
      else
        data[sel..".body3.com.comcolor"] =  65280
      end
      
      if(EmeterData[EmeterNum.."_4"] == 0) then
        data[sel..".body3.blackout.color"] =  16777215
      else
        data[sel..".body3.blackout.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_5"] == nil) then
        EmeterData[EmeterNum.."_5"] = 0
      else
        data[sel..".body3.RSvoltage.text"] = EmeterData[EmeterNum.."_5"].." [V]"
      end
      
      if(EmeterData[EmeterNum.."_6"] == nil) then
        EmeterData[EmeterNum.."_6"] = 0
      else
        data[sel..".body3.Rcurrent.text"] = EmeterData[EmeterNum.."_6"].." [A]"
      end
      
      if(EmeterData[EmeterNum.."_7"] == nil) then
        EmeterData[EmeterNum.."_7"] = 0
      else
        data[sel..".body3.STvoltage.text"] = EmeterData[EmeterNum.."_7"].." [V]"
      end
      
      if(EmeterData[EmeterNum.."_8"] == nil) then
        EmeterData[EmeterNum.."_8"] = 0
      else
        data[sel..".body3.Scurrent.text"] = EmeterData[EmeterNum.."_8"].." [A]"
      end
      
      if(EmeterData[EmeterNum.."_9"] == nil) then
        EmeterData[EmeterNum.."_9"] = 0
      else
        data[sel..".body3.TRvoltage.text"] = EmeterData[EmeterNum.."_9"].." [V]"
      end
      
      if(EmeterData[EmeterNum.."_10"] == nil) then
        EmeterData[EmeterNum.."_10"] = 0
      else
        data[sel..".body3.Tcurrent.text"] = EmeterData[EmeterNum.."_10"].." [A]"
      end
      
      if(EmeterData[EmeterNum.."_11"] == 0) then
        data[sel..".body3.RSovervoltage.color"] =  16777215
      else
        data[sel..".body3.RSovervoltage.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_12"] == 0) then
        data[sel..".body3.STovervoltage.color"] =  16777215
      else
        data[sel..".body3.STovervoltage.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_13"] == 0) then
        data[sel..".body3.TRovervoltage.color"] =  16777215
      else
        data[sel..".body3.TRovervoltage.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_14"] == 0) then
        data[sel..".body3.RSovercurrent.color"] =  16777215
      else
        data[sel..".body3.RSovercurrent.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_15"] == 0) then
        data[sel..".body3.STovercurrent.color"] =  16777215
      else
        data[sel..".body3.STovercurrent.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_16"] == 0) then
        data[sel..".body3.TRovercurrent.color"] =  16777215
      else
        data[sel..".body3.TRovercurrent.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_17"] == 0) then
        data[sel..".body3.Rout.color"] =  16777215
      else
        data[sel..".body3.Rout.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_18"] == 0) then
        data[sel..".body3.Sout.color"] =  16777215
      else
        data[sel..".body3.Sout.color"] =  16711680
      end
      
      if(EmeterData[EmeterNum.."_19"] == 0) then
        data[sel..".body3.Tout.color"] =  16777215
      else
        data[sel..".body3.Tout.color"] =  16711680
      end
    --[[-----------------]]--
  end
  end
  end
  
  gre.set_data(data)
end
--[[---------------------------]]--

--[[--------온습도 정보 파싱----------]]--
function Parsing_Temperature(info,line)
    
  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
    if(l==1) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==2) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==3) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==4) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==5) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==6) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==7) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==8) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==9) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==10) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==11) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==12) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==13) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==14) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==15) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==16) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==17) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==18) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==19) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==20) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==21) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==22) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==23) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==24) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==25) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==26) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==27) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==28) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==29) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==30) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==31) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==32) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==33) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==34) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==35) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==36) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==37) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==38) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==39) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==40) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==41) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==42) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==43) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==44) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==45) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==46) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==47) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==48) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==49) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==50) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==51) then
      TemperData[line.."_"..l] = tonumber(v)
    elseif(l==52) then
      TemperData[line.."_"..l] = tonumber(v)
    end
    gre.set_data(TemperData)
    end
end
--[[---------------------------]]--

--[[--------온습도 UI update----------]]--
function UI_Update_Temperature()
  local data = {}
  local body = {}
  local body2 = {}
  
  
  if (TemperNum >= TemperCount) then
    TemperNum = TemperCount
    data[sel..".arrow.arrowR.grd_x"] = 480
  else if (TemperNum <= 1) then
    TemperNum = 1
    data[sel..".arrow.arrowL.grd_x"] = -70
  end
  end
  
  
  if(TemperData[TemperNum.."_1"] == 0) then
    body["hidden"] = 0
    body2["hidden"] = 1
  else if(TemperData[TemperNum.."_1"] == 1) then
    body["hidden"] = 1
    body2["hidden"] = 0
  end
  end
  
  gre.set_group_attrs("temperature.body",body)
  gre.set_group_attrs("temperature.body2",body2)
  
  if(TemperData[TemperNum.."_1"] == 0) then
    
    if(TemperData[TemperNum.."_2"] == nil) then
      TemperData[TemperNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = TemperData[TemperNum.."_2"]
    end
    
    data[sel..".titlebar.devicenum2.text"] = "page"
    
    data[sel..".titlebar.devicename.text"] = "온도 감시"
        
    if(TemperData[TemperNum.."_3"] == nil) then
      TemperData[TemperNum.."_3"] = 0
    else
      data[sel..".body.device1.text"] = string.format("%03d",TemperData[TemperNum.."_3"])
    end
    
    if(TemperData[TemperNum.."_4"] == nil) then
      TemperData[TemperNum.."_4"] = 0
    else
      data[sel..".body.temp1.text"] = TemperData[TemperNum.."_4"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_5"] == 0) then
      data[sel..".body.com1.color"] =  16777215
    else
      data[sel..".body.com1.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_6"] == 0) then
      data[sel..".body.high1.comcolor"] =  16777215
    else
      data[sel..".body.high1.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_7"] == 0) then
      data[sel..".body.low1.comcolor"] =  16777215
    else
      data[sel..".body.low1.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_8"] == nil) then
      TemperData[TemperNum.."_8"] = 0
    else
      data[sel..".body.device2.text"] = string.format("%03d",TemperData[TemperNum.."_8"])
    end
    
    if(TemperData[TemperNum.."_9"] == nil) then
      TemperData[TemperNum.."_9"] = 0
    else
      data[sel..".body.temp2.text"] = TemperData[TemperNum.."_9"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_10"] == 0) then
      data[sel..".body.com2.color"] =  16777215
    else
      data[sel..".body.com2.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_11"] == 0) then
      data[sel..".body.high2.comcolor"] =  16777215
    else
      data[sel..".body.high2.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_12"] == 0) then
      data[sel..".body.low2.comcolor"] =  16777215
    else
      data[sel..".body.low2.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_13"] == nil) then
      TemperData[TemperNum.."_13"] = 0
    else
      data[sel..".body.device3.text"] = string.format("%03d",TemperData[TemperNum.."_13"])
    end
    
    if(TemperData[TemperNum.."_14"] == nil) then
      TemperData[TemperNum.."_14"] = 0
    else
      data[sel..".body.temp3.text"] = TemperData[TemperNum.."_14"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_15"] == 0) then
      data[sel..".body.com3.color"] =  16777215
    else
      data[sel..".body.com3.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_16"] == 0) then
      data[sel..".body.high3.comcolor"] =  16777215
    else
      data[sel..".body.high3.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_17"] == 0) then
      data[sel..".body.low3.comcolor"] =  16777215
    else
      data[sel..".body.low3.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_18"] == nil) then
      TemperData[TemperNum.."_18"] = 0
    else
      data[sel..".body.device4.text"] = string.format("%03d",TemperData[TemperNum.."_18"])
    end
    
    if(TemperData[TemperNum.."_19"] == nil) then
      TemperData[TemperNum.."_19"] = 0
    else
      data[sel..".body.temp4.text"] = TemperData[TemperNum.."_19"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_20"] == 0) then
      data[sel..".body.com4.color"] =  16777215
    else
      data[sel..".body.com4.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_21"] == 0) then
      data[sel..".body.high4.comcolor"] =  16777215
    else
      data[sel..".body.high4.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_22"] == 0) then
      data[sel..".body.low4.comcolor"] =  16777215
    else
      data[sel..".body.low4.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_23"] == nil) then
      TemperData[TemperNum.."_23"] = 0
    else
      data[sel..".body.device5.text"] = string.format("%03d",TemperData[TemperNum.."_23"])
    end
    
    if(TemperData[TemperNum.."_24"] == nil) then
      TemperData[TemperNum.."_24"] = 0
    else
      data[sel..".body.temp5.text"] = TemperData[TemperNum.."_24"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_25"] == 0) then
      data[sel..".body.com5.color"] =  16777215
    else
      data[sel..".body.com5.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_26"] == 0) then
      data[sel..".body.high5.comcolor"] =  16777215
    else
      data[sel..".body.high5.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_27"] == 0) then
      data[sel..".body.low5.comcolor"] =  16777215
    else
      data[sel..".body.low5.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_28"] == nil) then
      TemperData[TemperNum.."_28"] = 0
    else
      data[sel..".body.device6.text"] = string.format("%03d",TemperData[TemperNum.."_28"])
    end
    
    if(TemperData[TemperNum.."_29"] == nil) then
      TemperData[TemperNum.."_29"] = 0
    else
      data[sel..".body.temp6.text"] = TemperData[TemperNum.."_29"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_30"] == 0) then
      data[sel..".body.com6.color"] =  16777215
    else
      data[sel..".body.com6.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_31"] == 0) then
      data[sel..".body.high6.comcolor"] =  16777215
    else
      data[sel..".body.high6.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_32"] == 0) then
      data[sel..".body.low6.comcolor"] =  16777215
    else
      data[sel..".body.low6.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_33"] == nil) then
      TemperData[TemperNum.."_33"] = 0
    else
      data[sel..".body.device7.text"] = string.format("%03d",TemperData[TemperNum.."_33"])
    end
    
    if(TemperData[TemperNum.."_34"] == nil) then
      TemperData[TemperNum.."_34"] = 0
    else
      data[sel..".body.temp7.text"] = TemperData[TemperNum.."_34"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_35"] == 0) then
      data[sel..".body.com7.color"] =  16777215
    else
      data[sel..".body.com7.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_36"] == 0) then
      data[sel..".body.high7.comcolor"] =  16777215
    else
      data[sel..".body.high7.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_37"] == 0) then
      data[sel..".body.low7.comcolor"] =  16777215
    else
      data[sel..".body.low7.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_38"] == nil) then
      TemperData[TemperNum.."_38"] = 0
    else
      data[sel..".body.device8.text"] = string.format("%03d",TemperData[TemperNum.."_38"])
    end
    
    if(TemperData[TemperNum.."_39"] == nil) then
      TemperData[TemperNum.."_39"] = 0
    else
      data[sel..".body.temp8.text"] = TemperData[TemperNum.."_39"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_40"] == 0) then
      data[sel..".body.com8.color"] =  16777215
    else
      data[sel..".body.com8.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_41"] == 0) then
      data[sel..".body.high8.comcolor"] =  16777215
    else
      data[sel..".body.high8.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_42"] == 0) then
      data[sel..".body.low8.comcolor"] =  16777215
    else
      data[sel..".body.low8.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_43"] == nil) then
      TemperData[TemperNum.."_43"] = 0
    else
      data[sel..".body.device9.text"] = string.format("%03d",TemperData[TemperNum.."_43"])
    end
    
    if(TemperData[TemperNum.."_44"] == nil) then
      TemperData[TemperNum.."_44"] = 0
    else
      data[sel..".body.temp9.text"] = TemperData[TemperNum.."_44"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_45"] == 0) then
      data[sel..".body.com9.color"] =  16777215
    else
      data[sel..".body.com9.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_46"] == 0) then
      data[sel..".body.high9.comcolor"] =  16777215
    else
      data[sel..".body.high9.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_47"] == 0) then
      data[sel..".body.low9.comcolor"] =  16777215
    else
      data[sel..".body.low9.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_48"] == nil) then
      TemperData[TemperNum.."_48"] = 0
    else
      data[sel..".body.device10.text"] = string.format("%03d",TemperData[TemperNum.."_48"])
    end
    
    if(TemperData[TemperNum.."_49"] == nil) then
      TemperData[TemperNum.."_49"] = 0
    else
      data[sel..".body.temp10.text"] = TemperData[TemperNum.."_49"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_50"] == 0) then
      data[sel..".body.com10.color"] =  16777215
    else
      data[sel..".body.com10.color"] =  65280
    end
    
    if(TemperData[TemperNum.."_51"] == 0) then
      data[sel..".body.high10.comcolor"] =  16777215
    else
      data[sel..".body.high10.comcolor"] =  16711680
    end
    
    if(TemperData[TemperNum.."_52"] == 0) then
      data[sel..".body.low10.comcolor"] =  16777215
    else
      data[sel..".body.low10.comcolor"] =  16711680
    end
    
    

  else if(TemperData[TemperNum.."_1"] == 1) then
  
    if(TemperData[TemperNum.."_2"] == nil) then
      TemperData[TemperNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = TemperData[TemperNum.."_2"]
    end
    
    data[sel..".titlebar.devicenum2.text"] = "호기"
    
    data[sel..".titlebar.devicename.text"] = "온습도 감시 모듈"
        
    if(TemperData[TemperNum.."_3"] == 0) then
      data[sel..".body2.com.comcolor"] =  16777215
    else
      data[sel..".body2.com.comcolor"] =  65280
    end
    
    if(TemperData[TemperNum.."_4"] == nil) then
      TemperData[TemperNum.."_4"] = 0
    else
      data[sel..".body2.temp.text"] = TemperData[TemperNum.."_4"].." [℃]"
    end
    
    if(TemperData[TemperNum.."_5"] == nil) then
      TemperData[TemperNum.."_5"] = 0
    else
      data[sel..".body2.humi.text"] = TemperData[TemperNum.."_5"].." [%]"
    end
    
    if(TemperData[TemperNum.."_6"] == 0) then
      data[sel..".body2.hightemp.color"] =  16777215
    else
      data[sel..".body2.hightemp.color"] =  16711680
    end
    
    if(TemperData[TemperNum.."_7"] == 0) then
      data[sel..".body2.highhumi.color"] =  16777215
    else
      data[sel..".body2.highhumi.color"] =  16711680
    end
    
    if(TemperData[TemperNum.."_8"] == 0) then
      data[sel..".body2.lowtemp.color"] =  16777215
    else
      data[sel..".body2.lowtemp.color"] =  16711680
    end
    
    if(TemperData[TemperNum.."_9"] == 0) then
      data[sel..".body2.lowhumi.color"] =  16777215
    else
      data[sel..".body2.lowhumi.color"] =  16711680
    end
    
  end
  end
  gre.set_data(data)
end
--[[---------------------------]]--

--[[--------하론 정보 파싱----------]]--
function Parsing_Battery(info,line)
  local data = {}
  gre.set_data(data)
end
--[[---------------------------]]--

--[[--------축전지 정보 파싱----------]]--
function Parsing_ETC(info,line)
  
  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
  
  if(l==1) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==2) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==3) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==4) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==5) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==6) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==7) then
    ETCData[line.."_"..l] = tonumber(v)
  elseif(l==8) then
    ETCData[line.."_"..l] = tonumber(v)
  end

    gre.set_data(ETCData)
    end  
end
--[[---------------------------]]--

--[[--------기타 UI update----------]]--
function UI_Update_ETC()
  local data = {}
  local body = {}
  local body2 = {}
  local body3 = {}
  
  if (ETCNum >= ETCCount) then
    ETCNum = ETCCount
    data[sel..".arrow.arrowR.grd_x"] = 480
  else if (ETCNum <= 1) then
    ETCNum = 1
    data[sel..".arrow.arrowL.grd_x"] = -70
  end
  end
  
  if(ETCData[ETCNum.."_1"] == 0) then
    body["hidden"] = 0
    body2["hidden"] = 1
    body3["hidden"] = 1
  else if(ETCData[ETCNum.."_1"] == 1) then
    body["hidden"] = 1
    body2["hidden"] = 0
    body3["hidden"] = 1
  else if(ETCData[ETCNum.."_1"] == 2) then
    body["hidden"] = 1
    body2["hidden"] = 1
    body3["hidden"] = 0
  end
  end
  end
  
  gre.set_group_attrs("etc.body",body)
  gre.set_group_attrs("etc.body2",body2)
  gre.set_group_attrs("etc.body3",body3)
  
  
  if(ETCData[ETCNum.."_1"] == 0) then
  
  
    if(ETCData[ETCNum.."_2"] == nil) then
      ETCData[ETCNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ETCData[ETCNum.."_2"]
    end
      
    data[sel..".titlebar.devicename.text"] = "발동발전기"
  
    if(ETCData[ETCNum.."_3"] == 0) then
      data[sel..".body.com.comcolor"] =  16777215
    else
      data[sel..".body.com.comcolor"] =  65280
    end
          
    if(ETCData[ETCNum.."_4"] == nil) then
      ETCData[ETCNum.."_4"] = 0
    else
      if(ETCData[ETCNum.."_4"] == 1) then
        data[sel..".body.onoff.text"] = "RUN"
      else
        data[sel..".body.onoff.text"] = "STOP"
      end
    end
    
    if(ETCData[ETCNum.."_5"] == nil) then
      ETCData[ETCNum.."_5"] = 0
    else
      if(ETCData[ETCNum.."_5"] == 1) then
        data[sel..".body.fuelstate.text"] = "OK"
      else
        data[sel..".body.fuelstate.text"] = "FAIL"
      end
    end
    
    if(ETCData[ETCNum.."_6"] == nil) then
      ETCData[ETCNum.."_6"] = 0
    else
      if(ETCData[ETCNum.."_6"] == 1) then
        data[sel..".body.ATSstate.text"] = "OK"
      else
        data[sel..".body.ATSstate.text"] = "FAIL"
      end
    end
    
    if(ETCData[ETCNum.."_7"] == nil) then
      ETCData[ETCNum.."_7"] = 0
    else
      if(ETCData[ETCNum.."_7"] == 1) then
        data[sel..".body.actionstate.text"] = "OK"
      else
        data[sel..".body.actionstate.text"] = "FAIL"
      end
    end
    
  else if(ETCData[ETCNum.."_1"] == 1) then
    if(ETCData[ETCNum.."_2"] == nil) then
      ETCData[ETCNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ETCData[ETCNum.."_2"]
    end
      
    data[sel..".titlebar.devicename.text"] = "하론"
    
    if(ETCData[ETCNum.."_2"] == 0) then
      data[sel..".body2.ACstate.color"] =  16777215
    else
      data[sel..".body2.ACstate.color"] =  16711680
    end
    
    if(ETCData[ETCNum.."_3"] == 0) then
      data[sel..".body2.Release.color"] =  16777215
    else
      data[sel..".body2.Release.color"] =  16711680
    end
    
    if(ETCData[ETCNum.."_4"] == 0) then
      data[sel..".body2.DetectionA.color"] =  16777215
    else
      data[sel..".body2.DetectionA.color"] =  16711680
    end
    
    if(ETCData[ETCNum.."_5"] == 0) then
      data[sel..".body2.DetectionB.color"] =  16777215
    else
      data[sel..".body2.DetectionB.color"] =  16711680
    end
    
    
  else if(ETCData[ETCNum.."_1"] == 2) then
    if(ETCData[ETCNum.."_2"] == nil) then
      ETCData[ETCNum.."_2"] = 0
    else
      data[sel..".titlebar.devicenum.text"] = ETCData[ETCNum.."_2"]
    end
      
    data[sel..".titlebar.devicename.text"] = "인체감지/화재/침순"
    
    if(ETCData[ETCNum.."_3"] == 0) then
      data[sel..".body3.humandetection.color"] =  16777215
    else
      data[sel..".body3.humandetection.color"] =  16711680
    end
    
    if(ETCData[ETCNum.."_4"] == 0) then
      data[sel..".body3.fireseneor.color"] =  16777215
    else
      data[sel..".body3.fireseneor.color"] =  16711680
    end
    
    if(ETCData[ETCNum.."_5"] == 0) then
      data[sel..".body3.floodingsensor.color"] =  16777215
    else
      data[sel..".body3.floodingsensor.color"] =  16711680
    end
    
  end
  end
  end
    
  gre.set_data(data)
end
--[[---------------------------]]--

--[[--------출입문 정보 파싱----------]]--
function Parsing_Entrance(info,line)
  
  readdata = string.split(info, ",")
  for l, v in pairs(readdata) do
    if(l==1) then
      EntranceData[line.."_"..l] = tonumber(v)
    elseif(l==2) then
      EntranceData[line.."_"..l] = v
    elseif(l==3) then
      EntranceData[line.."_"..l] = v
    end
    gre.set_data(EntranceData)
    end  

end
--[[---------------------------]]--

--[[--------출입 UI update----------]]--
function UI_Update_Entrance()
  local data = {}
  local body = {}
  
  if (EntranceNum >= EntranceCount) then
    EntranceNum = EntranceCount
    data[sel..".arrow.arrowR.grd_x"] = 480
  else if (EntranceNum <= 1) then
    EntranceNum = 1
    data[sel..".arrow.arrowL.grd_x"] = -70
  end
  end
  
  if(EntranceData[EntranceNum.."_1"] == nil) then
    EntranceData[EntranceNum.."_1"] = 0
  else
    data[sel..".titlebar.devicenum.text"] = EntranceData[EntranceNum.."_1"]
  end
    
    
  data[sel..".titlebar.devicename.text"] = "출입"
        
  if(EntranceData[EntranceNum.."_2"] == nil) then
    EntranceData[EntranceNum.."_2"] = 0
  else
    if(EntranceData[EntranceNum.."_2"] == 0) then
      data[sel..".body.doorstate.text"] = "CLOSE"
    else
      data[sel..".body.doorstate.text"] = "OPEN"
    end
  end
  
  if(EntranceData[EntranceNum.."_3"] == nil) then
    EntranceData[EntranceNum.."_3"] = 0
  else
    data[sel..".body.rfkey.text"] = EntranceData[EntranceNum.."_3"]
  end
    
  gre.set_data(data)
end
--[[---------------------------]]--


--[[--------설정 정보 파싱----------]]--
function Parsing_Setup(info,line)
  local data = {}
  gre.set_data(data)
end
--[[---------------------------]]--


function Leftbtn_Rectifier (mapargs) 

  local data = {}
  
  if(RectCount == 2) then
    if(RectNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  else
    if(RectNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  end
  
  RectNum = RectNum-1
  UI_Update_Rect()
  gre.set_data(data)
end

function Rightbtn_Rectifier (mapargs) 

  local data = {}
  
  if(RectCount == 2) then
    if(RectNum == RectCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
      data[sel..".arrow.arrowL.grd_x"] = 0
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  else
    if(RectNum == RectCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  end
  
  
  RectNum = RectNum+1
  UI_Update_Rect()
  gre.set_data(data)
end

function Leftbtn_Aircon (mapargs) 

  local data = {}
  
  if(ACCount == 2) then
    if(ACNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  else
    if(ACNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
    else
      --data[sel..".arrow.arrowR.grd_x"] = 430
      data[sel..".arrow.arrowR.grd_x"] = 410
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  end

  ACNum = ACNum-1
  UI_Update_Aircon()
  gre.set_data(data)
end

function Rightbtn_Aircon (mapargs) 

  local data = {}
  
  if(ACCount == 2) then
    if(ACNum == ACCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
      data[sel..".arrow.arrowL.grd_x"] = 0
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  else
    if(ACNum == ACCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  end
  
  ACNum = ACNum+1  
  UI_Update_Aircon()
  gre.set_data(data)
end

function Leftbtn_Emeter (mapargs) 

  local data = {}
  
  if(EmeterCount == 2) then
    if(EmeterNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  else
    if(EmeterNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  end

  EmeterNum = EmeterNum-1
  UI_Update_Emeter()
  gre.set_data(data)
end

function Rightbtn_Emeter (mapargs) 

  local data = {}
  
  if(EmeterCount == 2) then
    if(EmeterNum == EmeterCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
      data[sel..".arrow.arrowL.grd_x"] = 0
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  else
    if(EmeterNum == EmeterCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  end

  EmeterNum = EmeterNum+1  
  UI_Update_Emeter()
  gre.set_data(data)
end

function Leftbtn_Temperature (mapargs) 

  local data = {}
  
  if(TemperCount == 2) then
    if(TemperNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  else
    if(TemperNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  end
  
  TemperNum = TemperNum-1
  UI_Update_Temperature()
  gre.set_data(data)
end

function Rightbtn_Temperature (mapargs) 

  local data = {}
  
  if(TemperCount == 2) then
    if(TemperNum == TemperCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
      data[sel..".arrow.arrowL.grd_x"] = 0
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  else
    if(TemperNum == TemperCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  end
   
  TemperNum = TemperNum+1  
  UI_Update_Temperature()
  gre.set_data(data)
end

function Leftbtn_ETC (mapargs) 

  local data = {}
  
  if(ETCCount == 2) then
    if(ETCNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  else
    if(ETCNum == 2) then
      data[sel..".arrow.arrowL.grd_x"] = -70
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
      data[sel..".arrow.arrowL.grd_x"] = 0
    end
  end
  
  ETCNum = ETCNum-1
  UI_Update_ETC()
  gre.set_data(data)
end

function Rightbtn_ETC (mapargs) 

  local data = {}
  
  if(ETCCount == 2) then
    if(ETCNum == ETCCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
      data[sel..".arrow.arrowL.grd_x"] = 0
    else
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  else
    if(ETCNum == ETCCount-1) then
      data[sel..".arrow.arrowR.grd_x"] = 480
    else
      data[sel..".arrow.arrowL.grd_x"] = 0
      data[sel..".arrow.arrowR.grd_x"] = 410
      --data[sel..".arrow.arrowR.grd_x"] = 430
    end
  end
   
  ETCNum = ETCNum+1  
  UI_Update_ETC()
  gre.set_data(data)
end


function main (mapargs)
  local data = {}
  local comstate
  local almstate
  
  --[[-- 통신,알람 색상표시 --]]--
  local fimain = io.open("/mnt/ramdisk/main.ntx","r")
  
  if(fimain ~= nil) then  
    for fields in fimain:lines() do
      readdata = string.split(fields, ",")
      for l, v in pairs(readdata) do
        if(l==4) then
         data["Main.titlebar.com.comstate"]  = tonumber(v)
        elseif(l==5) then
         data["Main.titlebar.alm.almstate"]  = tonumber(v)
        end
        --gre.set_data(data)
      end
    end
  end
  --[[---------------------------------------]]--
  comstate = gre.get_value("Main.titlebar.com.comstate")
  almstate = gre.get_value("Main.titlebar.alm.almstate")
  
  if(comstate == 0) then
    data["Main.titlebar.com.comcolor"] = 16777215
  else
    data["Main.titlebar.com.comcolor"] = 65280
  end
  
  if(almstate == 0) then
    data["Main.titlebar.alm.almcolor"] = 16777215
  else
    data["Main.titlebar.alm.almcolor"] = 16711680
  end
  --[[------------------]]--
  
  gre.set_data(data)
    
end

function bottombar_timer (mapargs)

  local data = {}
  local today = os.date("*t")
  
  --[[-- 시간데이터 표시 --]]--
  
  data["date"] = today["year"].."-"..os.date("%m").."-"..os.date("%d")
  data["time"] = os.date("%X")

  data["Main.bottombar.date.text"] = data["date"]
  data["Main.bottombar.time.text"] = data["time"]
  data["rectifier.bottombar.date.text"] = data["date"]
  data["rectifier.bottombar.time.text"] = data["time"]
  data["emeter.bottombar.date.text"] = data["date"]
  data["emeter.bottombar.time.text"] = data["time"]
  data["aircon.bottombar.date.text"] = data["date"]
  data["aircon.bottombar.time.text"] = data["time"]
  data["temperature.bottombar.date.text"] = data["date"]
  data["temperature.bottombar.time.text"] = data["time"]
  data["entrance.bottombar.date.text"] = data["date"]
  data["entrance.bottombar.time.text"] = data["time"]
  data["etc.bottombar.date.text"] = data["date"]
  data["etc.bottombar.time.text"] = data["time"]
  data["hyphen.bottombar.date.text"] = data["date"]
  data["hyphen.bottombar.time.text"] = data["time"]
  data["setup.bottombar.date.text"] = data["date"]
  data["setup.bottombar.time.text"] = data["time"]
  --[[---------------]]--
  
  gre.set_data(data)
  
end