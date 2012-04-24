#!/usr/bin/env ruby

Chunk = Struct.new(:sub, :max_id, :text)

CHUNKS = []

CHUNKS << Chunk.new(38, 68, <<EOF
__text:00406CC3                 movzx   ecx, word ptr [ecx]       ; AFTER: ecx  = skill id
__text:00406CC6                 sub     ecx, 38
__text:00406CC9                 cmp     cx, 68
__text:00406CCD                 ja      not_want_combat
__text:00406CD3                 mov     ebp, [esp+4Ch+var_3C]     ; ebp = 406ce4
__text:00406CD7                 movzx   ecx, cx
__text:00406CDA                 mov     ecx, [ebp+ecx*4+0]
__text:00406CDE                 add     ecx, ebx                  ; ebx = 0x406c19
__text:00406CE0                 jmp     ecx                       ; 406e90
__text:00406CE0 ; ---------------------------------------------------------------------------
__text:00406CE2                 align 4
__text:00406CE4 off_406CE4      dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                                                   ; DATA XREF: CreatureInfoScreen_prepare+87o
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset not_want_combat - offset loc_406C19
__text:00406CE4                 dd offset not_want_combat - offset loc_406C19  ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19        ,offset is_combat - offset loc_406C19
__text:00406CE4                 dd offset is_combat - offset loc_406C19
EOF
)

CHUNKS << Chunk.new( 0, 115, <<EOF
__text:00406EE3                 cmp     word ptr [ecx], 115
__text:00406EE7                 ja      not_want_labor
__text:00406EED                 movzx   ecx, word ptr [ecx]
__text:00406EF0                 mov     ebp, [esp+4Ch+var_3C]
__text:00406EF4                 mov     ecx, [ebp+ecx*4+0]
__text:00406EF8                 add     ecx, ebx
__text:00406EFA                 jmp     ecx
__text:00406EFA ; ---------------------------------------------------------------------------
__text:00406EFC off_406EFC      dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                                                   ; DATA XREF: CreatureInfoScreen_prepare+2A8o
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset not_want_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset not_want_labor - offset loc_406C19   ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
__text:00406EFC                 dd offset is_labor - offset loc_406C19         ,offset is_labor - offset loc_406C19
EOF
)

CHUNKS << Chunk.new( 56, 52, <<EOF
__text:00407156                 sub     ecx, 56
__text:00407159                 cmp     cx, 52
__text:0040715D                 ja      not_want_misc
__text:00407163                 mov     ebp, [esp+4Ch+var_3C]
__text:00407167                 movzx   ecx, cx
__text:0040716A                 mov     ecx, [ebp+ecx*4+0]
__text:0040716E                 add     ecx, ebx
__text:00407170                 jmp     ecx
__text:00407170 ; ---------------------------------------------------------------------------
__text:00407172                 align 4
__text:00407174 off_407174      dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                                                   ; DATA XREF: CreatureInfoScreen_prepare+51Co
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset is_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19          ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset not_want_misc - offset loc_406C19    ,offset not_want_misc - offset loc_406C19
__text:00407174                 dd offset is_misc - offset loc_406C19
EOF
)

def check_chunk skill_id, chunk
  offsets = chunk.text.scan(/offset \w+ - offset \w+/)
  raise if offsets.size != chunk.max_id+1
  skill_id -= chunk.sub
  return nil if skill_id < 0 || skill_id > chunk.max_id
  offsets[skill_id]
end

def check_chunks skill_id
  CHUNKS.each do |c|
    r = check_chunk(skill_id, c)
    if r && r[' is_']
      return r[/is_\w+/].sub('is_','').upcase
    end
  end
  nil
end

h = Hash.new{ |k,v| k[v] = [] }
a = []

0.upto(200) do |skill_id|
  if r = check_chunks(skill_id)
    printf "[.] %2x = %s\n", skill_id, r
    h[r] << skill_id
    a[skill_id] = r
  end
end

puts(a.map do |x|
  case x
  when 'COMBAT'; 1
  when 'LABOR';  2
  when 'MISC';   3
  else raise
  end
end.join(','))
