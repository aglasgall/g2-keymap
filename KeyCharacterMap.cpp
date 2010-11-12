
#include "KeyCharacterMap.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>


struct Header
{
    char magic[8];
    unsigned int endian;
    unsigned int version;
    unsigned int keycount;
    unsigned char kbdtype;
    char padding[11];
};

KeyCharacterMap::KeyCharacterMap()
{
}

KeyCharacterMap::~KeyCharacterMap()
{
    free(m_keys);
}

unsigned short
KeyCharacterMap::get(int keycode, int meta)
{
    Key* k = find_key(keycode);
    if (k != NULL) {
        return k->data[meta & META_MASK];
    }
    return 0;
}

unsigned short
KeyCharacterMap::getNumber(int keycode)
{
    Key* k = find_key(keycode);
    if (k != NULL) {
        return k->number;
    }
    return 0;
}

unsigned short
KeyCharacterMap::getMatch(int keycode, const unsigned short* chars,
                          int charsize, uint32_t modifiers)
{
    Key* k = find_key(keycode);
    modifiers &= 3; // ignore the SYM key because we don't have keymap entries for it
    if (k != NULL) {
        const uint16_t* data = k->data;
        for (int j=0; j<charsize; j++) {
            uint16_t c = chars[j];
            for (int i=0; i<(META_MASK + 1); i++) {
                if ((modifiers == 0) || ((modifiers & i) != 0)) {
                    if (c == data[i]) {
                        return c;
                    }
                }
            }
        }
    }
    return 0;
}

unsigned short
KeyCharacterMap::getDisplayLabel(int keycode)
{
    Key* k = find_key(keycode);
    if (k != NULL) {
        return k->display_label;
    }
    return 0;
}

bool
KeyCharacterMap::getKeyData(int keycode, unsigned short *displayLabel,
                            unsigned short *number, unsigned short* results)
{
    Key* k = find_key(keycode);
    if (k != NULL) {
        memcpy(results, k->data, sizeof(short)*(META_MASK + 1));
        *number = k->number;
        *displayLabel = k->display_label;
        return true;
    } else {
        return false;
    }
}

bool
KeyCharacterMap::find_char(uint16_t c, uint32_t* key, uint32_t* mods)
{
    uint32_t N = m_keyCount;
    for (int j=0; j<(META_MASK + 1); j++) {
        Key const* keys = m_keys;
        for (uint32_t i=0; i<N; i++) {
            if (keys->data[j] == c) {
                *key = keys->keycode;
                *mods = j;
                return true;
            }
            keys++;
        }
    }
    return false;
}


KeyCharacterMap::Key*
KeyCharacterMap::find_key(int keycode)
{
    Key* keys = m_keys;
    int low = 0;
    int high = m_keyCount - 1;
    int mid;
    int n;
    while (low <= high) {
        mid = (low + high) / 2;
        n = keys[mid].keycode;
        if (keycode < n) {
            high = mid - 1;
        } else if (keycode > n) {
            low = mid + 1;
        } else {
            return keys + mid;
        }
    }
    return NULL;
}

KeyCharacterMap*
KeyCharacterMap::try_file(const char* filename)
{
    KeyCharacterMap* rv = NULL;
    Key* keys;
    int fd;
    off_t filesize;
    Header header;
    int err;
    
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr,"Can't open keycharmap file");
        return NULL;
    }

    filesize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    // validate the header
    if (filesize <= (off_t)sizeof(header)) {
        fprintf(stderr,"Bad keycharmap - filesize=%d\n", (int)filesize);
        goto cleanup1;
    }

    err = read(fd, &header, sizeof(header));
    if (err == -1) {
        fprintf(stderr,"Error reading keycharmap file");
        goto cleanup1;
    }

    if (0 != memcmp(header.magic, "keychar", 8)) {
        fprintf(stderr,"Bad keycharmap magic token");
        goto cleanup1;
    }
    if (header.endian != 0x12345678) {
        fprintf(stderr,"Bad keycharmap endians");
        goto cleanup1;
    }
    if ((header.version & 0xff) != 2) {
        fprintf(stderr,"Only support keycharmap version 2 (got 0x%08x)", header.version);
        goto cleanup1;
    }
    if (filesize < (off_t)(sizeof(Header)+(sizeof(Key)*header.keycount))) {
        fprintf(stderr,"Bad keycharmap file size\n");
        goto cleanup1;
    }

    // read the key data
    keys = (Key*)malloc(sizeof(Key)*header.keycount);
    err = read(fd, keys, sizeof(Key)*header.keycount);
    if (err == -1) {
        fprintf(stderr,"Error reading keycharmap file");
        free(keys);
        goto cleanup1;
    }

    // return the object
    rv = new KeyCharacterMap;
    rv->m_keyCount = header.keycount;
    rv->m_keys = keys;
    rv->m_type = header.kbdtype;

cleanup1:
    close(fd);

    return rv;
}
