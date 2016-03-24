#!/usr/bin/env python

from os import listdir, remove
from os.path import isfile, join

ifguard = 'SHADERS_H_'

h_heading = '#ifndef %s\n#define %s\n\n' % (ifguard, ifguard)
h_ending = '\n#endif /* %s */\n' % (ifguard)

s_heading = '#include "shaders.h"\n\n'

shader_def = 'extern const char * %s;\n'
shader_imp_s = 'const char * %s = {\n'
shader_imp_line = '  "%s\\n"\n'
shader_imp_e = '};\n\n'

def generate_c_shaders(shader_path, header_path, src_path):
  shader_filenames = [f for f in listdir(shader_path) if isfile(join(shader_path, f))]

  if isfile(header_path):
    remove(header_path)
  header = open(header_path, 'w')

  if isfile(src_path):
    remove(src_path);
  src = open(src_path, 'w')

  header.write(h_heading)
  src.write(s_heading)

  for filename in shader_filenames:
    shader_contents = open(join(shader_path, filename), 'r')
    shader_name = filename.upper().replace('.', '_')

    header.write(shader_def % shader_name)

    src.write(shader_imp_s % shader_name)
    for line in shader_contents:
      src.write(shader_imp_line % line.rstrip()) if line.rstrip() else 0
    src.write(shader_imp_e)

    shader_contents.close()

  header.write(h_ending)
  header.close()

  src.close()

if __name__ == '__main__':
  generate_c_shaders('shaders', 'include/shaders.h', 'src/shaders.c');
