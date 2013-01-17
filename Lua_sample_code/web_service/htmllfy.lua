#!/usr/bin/env wsapi.cgi

require"orbit"
require "luasql.mysql"

module("htmllfy", package.seeall, orbit.new)

function wrap (inner)
	return html{ head(), body(inner) }
end

function test ()
	return wrap(form ({action="test.lua", H'table' {
		tr{td{"First name"},td( input{type='text', name='first'})},
		tr{td{"Second name"},td(input{type='text', name='second'})},
		tr{ td(input{type='submit', value='Submit!'}),
		td(input{type='submit',value='Cancel'})
	},
}}))
end

htmllfy:dispatch_get(test, "/")

function control_param(web,arg)
	return html{ head(), body(
	h1{"input="..arg}
	) } 
end

htmllfy:dispatch_get(control_param, "/input/(.+)")

-- htmllfy.lua/cgiget?author=sssssss&comment=fdkjsdklfjsd&email=w&url=wqq
function cgiget(web)
	return html{ 
		head{
			title{"Blog Title"},
			meta{ ["http-equiv"] = "Content-Type",
			content = "text/html; charset=utf-8" },
			link{ rel = 'stylesheet', type = 'text/css', 
			href = web:static_link('/style.css'), media = 'screen' }	
		}, body{
	    {
			div{class="blogentry",
		h1{"cgiget = "..web.input.comment},
		h1{"cgiget = "..web.input.author},
		h1{"cgiget = "..web.input.email},
		h1{"cgiget = "..web.input.url}
	    }
	}
	} } 
end

htmllfy:dispatch_get(cgiget, "/cgiget")

function redirect_to_yahoo(web)
	return 	web:redirect("http://tw.yahoo.com")
end

htmllfy:dispatch_get(redirect_to_yahoo, "/redirect")



function generate_xml(web)
	web.headers["Content-Type"] = "application/xml"
	return sensors {
		sensor {name="one"},
		sensor {name="two"},
		sensor {name="3"},
		sensor {name="4"},
		sensor {name="5"},
	}
end

htmllfy:dispatch_get(generate_xml, "/generate_xml")

function rows (connection, sql_statement)
  local cursor = assert (connection:execute (sql_statement))

  local names, types = cursor:getcolnames(), cursor:getcoltypes()
  print (unpack(names))
  print (unpack(types))
  return function ()
    return cursor:fetch()
  end
end

function showdatabase(web)
	env = assert (luasql.mysql())
	con = assert (env:connect("my_test","mark","jadigrxl"))
	local trHtml = {}
	for CustomerID in rows (con, "show columns from Customers") do
		--print (string.format ("%s \n", CustomerID))
		trHtml[#trHtml+1] = td(CustomerID)
	end
	local innerHtml = {}
	for CustomerID, CompanyName, ContactName, Address, City, PostalCode, Country in rows (con, "select * from Customers") do
		--print (string.format ("%s %s %s %s %s %s %s\n", CustomerID, CompanyName, ContactName, Address, City, PostalCode, Country))
		innerHtml[#innerHtml+1] = td(CustomerID)
		innerHtml[#innerHtml+1] = td(CompanyName)
		innerHtml[#innerHtml+1] = td(ContactName)
		innerHtml[#innerHtml+1] = td(Address)
		innerHtml[#innerHtml+1] = td(City)
		innerHtml[#innerHtml+1] = td(PostalCode)
		innerHtml[#innerHtml+1] = td(Country)
		innerHtml[#innerHtml+1] = tr()
	end

	-- close everything
	con:close()
	env:close()

	return html{ 
		head{
			title{"Blog Title"},
			meta{ ["http-equiv"] = "Content-Type",
			content = "text/html; charset=utf-8" },
			link{ rel = 'stylesheet', type = 'text/css', 
			href = web:static_link('/style.css'), media = 'screen' }	
		}, body{
	    {
			div{class="blogentry",
	        H'table'{ id = "contents", tr{trHtml},tr{innerHtml} },
	    }
	}
	} } 

end

htmllfy:dispatch_get(showdatabase, "/showdatabase")

function showjsonformat(web)
	web.headers["Content-Type"] = "application/json"
	env = assert (luasql.mysql())
	con = assert (env:connect("my_test","mark","jadigrxl"))
	local json_sql = [[
	SELECT CONCAT("[",
	GROUP_CONCAT(
	CONCAT("{\"customerid\":","\"",CustomerID,"\""),
	CONCAT(",","\"companyname\":","\"",CompanyName,"\"}"))
	,"]")
	AS json FROM Customers;
	]]

	local jsondata 
    for json in rows (con, json_sql)
	do 
		jsondata = json
	end
	-- close everything
	con:close()
	env:close()

	return jsondata
end
htmllfy:dispatch_get(showjsonformat, "/showjsonformat")

orbit.htmlify(htmllfy, wrap,test, control_param, cgiget, redirect_to_yahoo, generate_xml, showdatabase)
