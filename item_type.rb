#!/usr/bin/env ruby

types = {}

types[ 0] = "bars"
types[ 1] = "small cut gems"
types[ 2] = "blocks"
types[ 3] = "rough gems"
types[ 4] = "stones"
types[ 5] = "logs"
types[ 6] = "doors"
types[ 7] = "floodgates"
types[ 8] = "beds"
types[ 9] = "thrones"
types[10] = "chains & ropes"
types[11] = "flasks"
types[12] = "goblets"
types[13] = "musical instruments"
types[14] = "toys"
types[16] = "cages"
types[17] = "barrels"
types[18] = "buckets"
types[20] = "tables"
types[21] = "coffins"
types[22] = "statues"
types[23] = "corpses"
types[24] = "weapons"
types[25] = "bodywear"
types[26] = "footwear"
types[27] = "shields"
types[28] = "headwear"
types[29] = "handwear"
types[30] = "boxes & bags"
types[31] = "bins"
types[32] = "armor stands"
types[33] = "weapon racks"
types[34] = "cabinets"
types[35] = "figurines"
types[36] = "amulets"
types[37] = "scepters"
types[38] = "ammunition"
types[39] = "crowns"
types[40] = "rings"
types[41] = "earrings"
types[42] = "bracelets"
types[43] = "large cut gems"
types[44] = "anvils"
types[45] = "body parts"
types[46] = "remains"
types[47] = "meat"
types[48] = "fish"
types[52] = "seeds"
types[53] = "plants"
types[54] = "leather"
types[56] = "thread"
types[57] = "cloth"
types[59] = "legwear"
types[60] = "backpacks"
types[61] = "quivers"
types[62] = "catapult parts"
types[66] = "mechanisms"
types[67] = "trap components"
types[68] = "drinks"
types[69] = "powder"
types[70] = "cheese"
types[71] = "prepared meals"
types[72] = "liquid"
types[73] = "coins"
types[74] = "glob"
types[76] = "pipe sections"
types[77] = "hatch covers"
types[78] = "grates"
types[79] = "querns"
types[81] = "splints"
types[82] = "crutches"
types[83] = "traction benches"
types[85] = "tools"
types[86] = "slabs"
types[87] = "eggs"
types[88] = "books"

def desc2const desc
  desc.upcase.gsub(/[^A-Z0-9]+/,'_')
end

puts "class ItemType {"
puts "    public:"
puts

types.each do |id,name|
  next unless name
  const_name = desc2const(name)
  printf "    static const int %-22s = %3d;\n", const_name, id
end

puts
printf "    static const char* type2string(int type_id){\n"
printf "        switch(type_id){\n"

types.each do |id,name|
  next unless name
  const_name = desc2const(name)
  printf "            case %-22s: return %s;\n", const_name, name.gsub("&","&amp;").inspect
end

puts "        }"
puts "        static char buf[0x100];\n        sprintf(buf, \"type %d\", type_id);\n        return buf;"
puts "    }"
puts "};"
