#include <stdio.h>
#include <stdlib.h>
#include "KeyCharacterMap.h"
#include "KeycodeLabels.h"
#include <ctype.h>

int compare_keycode_labels(const void *key, const void *elem) {
  int keycode = *(int*)key;
  KeycodeLabel *label = (KeycodeLabel*)elem;
  if (keycode < label->value) {
    return -1;
  } else if (keycode == label->value) {
    return 0;
  } else if (keycode > label->value) {
    return 1;
  }
}

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("usage: %s <kcm.bin file>\n",argv[0]);
    return 1;
  }
  KeyCharacterMap *map = KeyCharacterMap::try_file(argv[1]);
  printf("# of keys: %d\n",map->m_keyCount);
  for(int i = 0; i < map->m_keyCount; ++i) {
    KeyCharacterMap::Key key = map->m_keys[i];
    KeycodeLabel* label = (KeycodeLabel*)bsearch(&(key.keycode), KEYCODES,105,
                                  sizeof(KeycodeLabel),
                                  compare_keycode_labels);
    if(label) {
      printf("%s\t",label->literal);
    } else {
      printf("<UNKNOWN:%d>\t",key.keycode);
    }
    for(int j = 0; j < META_MASK; ++j) {
      int keychar = key.data[j];
      if(isascii(keychar) && isgraph(keychar)) {
          printf("%c\t",keychar);      
        } else {
          printf("%#x\t", keychar);
        }
    }
    printf("%c\t",key.number);
    printf("%c\n",key.display_label);
  }
  delete map;
  return 0;

}
