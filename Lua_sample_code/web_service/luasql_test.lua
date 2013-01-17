require "luasql.mysql"

function rows (connection, sql_statement)
  local cursor = assert (connection:execute (sql_statement))

  local names, types = cursor:getcolnames(), cursor:getcoltypes()
  print (unpack(names))
  print (unpack(types))
  return function ()
    return cursor:fetch()
  end
end

env = assert (luasql.mysql())
con = assert (env:connect("my_test","mark","jadigrxl"))
for CustomerID in rows (con, "show columns from Customers") do
  print (string.format ("%s \n", CustomerID))
end
for CustomerID, CompanyName, ContactName, Address, City, PostalCode, Country in rows (con, "select * from Customers") do
  print (string.format ("%s %s %s %s %s %s %s\n", CustomerID, CompanyName, ContactName, Address, City, PostalCode, Country))
end

-- close everything
con:close()
env:close()
