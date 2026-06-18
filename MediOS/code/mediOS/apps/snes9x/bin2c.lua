local i
format = string.format
local function printf(...)
   print(format(unpack(arg)))
end

local names = { }
for i=1, table.getn(arg) do
   printf("// %s", arg[i])
   local n = string.gsub(arg[i], "[.-/]", "_")
   local fn = arg[i]
   names[i] = n
   printf("char %s_data[] = {", n)
   local f = io.open(fn, "rb")
   local c = f:read("*a")
   local k = 1
   while k<=string.len(c) do
      local s = "  "
      local j
      for j=1, 20 do
	 s = s..tostring(string.byte(c, k))..","
	 k = k+1
	 if k>string.len(c) then break end
      end
      print(s)
   end
   f:close()
   print("};")
end

print("struct { char * name, * chunk; int size; } chunks[] = {")
for i=1, table.getn(arg) do
   local n = names[i]
   printf('  "%s", %s_data, sizeof(%s_data), /* %s */', n, n, n, arg[i])
end
print("};")
