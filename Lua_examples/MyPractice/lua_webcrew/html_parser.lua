--[[
HTML Parser in Lua
Inspired by this C# code:
http://www.codeproject.com/useritems/HTML_to_Plain_Text.asp

It does exactly the same as described there.
Only converts offline files.

Distributed under a modified BSD license, see at the end of the file
]]

-- What is the desired newline?
nl = "\n"
count = 1
function HTML_ToText (file)
  -- Declare variables, load the file. Make tags lowercase.
  local text
  local f=io.open (file)
  if f then
    text = f:read ("*a")
    f:close()
  end
  text = string.gsub (text,"(%b<>)",
  function (tag)
    return tag:lower()
  end)
  --[[ 
  First we kill the developer formatting (tabs, CR, LF)
  and produce a long string with no newlines and tabs.
  We also kill repeated spaces as browsers ignore them anyway.
  ]]
  
  
  local devkill=
    {
      ["(".."\n"..")"] = " ",
      ["(".."\t"..")"] = " ",
      --["("..string.char(15)..")"] = "",
      ["(%s%s+)"]=" ",
    }
  for pat, res in pairs (devkill) do
    text = string.gsub (text, pat, res)
  end
  
  
  -- Then we remove the header. We do this by stripping it first.
  text = string.gsub (text, "(<%s*head[^>]*>)", "<head>")
  text = string.gsub (text, "(<%s*%/%s*head%s*>)", "</head>")
  text = string.gsub (text, "(<head>,*<%/head>)", "")
  -- Kill all scripts. First we nuke their attribs.
  text = string.gsub (text, "(<%s*script[^>]*>)", "<script>")
  text = string.gsub (text, "(<%s*%/%s*script%s*>)", "</script>")
  text = string.gsub (text, "(<script>,*<%/script>)", "")
  -- Ok, same for styles.
  text = string.gsub (text, "(<%s*style[^>]*>)", "<style>")
  text = string.gsub (text, "(<%s*%/%s*style%s*>)", "</style>")
  text = string.gsub (text, "(<style>.*<%/style>)", "")
  
  -- Replace <td> with tabulators.
  text = string.gsub (text, "(<%s*td[^>]*>)","\t")
  
  -- Replace <br> with linebreaks.
  text = string.gsub (text, "(<%s*br%s*%/%s*>)",nl)
  
  -- Replace <li> with an asterisk surrounded by 2 spaces.
  -- Replace </li> with a newline.
  --print(text)
  for text1,text2,text3 in string.gmatch(text, "(<%s*li%s*%s*>)(.-)(<%s*/%s*li%s*%s*>)")
  do
  	text2 = string.gsub(text2,"%b<>","")
  	text2 = string.gsub(text2,"&nbsp;","")
  	print(count.."\n"..text2)
  	  count = count + 1
  end

  text = string.gsub (text, "(<%s*/%s*li%s*%s*>)",nl)
  --print(text)
  -- <p>, <div>, <tr>, <ul> will be replaced to a double newline.
    text = string.gsub (text, "(<%s*div[^>]*>)", nl..nl)
    text = string.gsub (text, "(<%s*p[^>]*>)", nl..nl)
    text = string.gsub (text, "(<%s*tr[^>]*>)", nl..nl)
    text = string.gsub (text, "(<%s*%/*%s*ul[^>]*>)", nl..nl)
  -- 
  
  -- Nuke all other tags now.
  text = string.gsub (text, "(%b<>)","")
   --print(text)
  -- Replace entities to their correspondant stuff where applicable.
  -- C# is owned badly here by using a table. :-P
  -- A metatable secures entities, so you can add them natively as keys.
  -- Enclosing brackets also get added automatically (capture!)
  local entities = {}
  setmetatable (entities,
  {
    __newindex = function (tbl, key, value)
      key = string.gsub (key, "(%#)" , "%%#")
      key = string.gsub (key, "(%&)" , "%%&")
      key = string.gsub (key, "(%;)" , "%%;")
      key = string.gsub (key, "(.+)" , "("..key..")")
      rawset (tbl, key, value)
    end
  })
  entities = 
  {
    ["&nbsp;"] = " ",
    ["&bull;"] = " *  ",
    ["&lsaquo;"] = "<",
    ["&rsaquo;"] = ">",
    ["&trade;"] = "(tm)",
    ["&frasl;"] = "/",
    ["&lt;"] = "<",
    ["&gt;"] = ">",
    ["&copy;"] = "(c)",
    ["&reg;"] = "(r)",
    -- Then kill all others.
    -- You can customize this table if you would like to, 
    -- I just got bored of copypasting. :-)
    -- http://hotwired.lycos.com/webmonkey/reference/special_characters/
    ["%&.+%;"] = "",
  }
  for entity, repl in pairs (entities) do
    text = string.gsub (text, entity, repl)
  end
  
  return text
  
end

result = HTML_ToText ("台中市北屯區.html")
--print(result)

