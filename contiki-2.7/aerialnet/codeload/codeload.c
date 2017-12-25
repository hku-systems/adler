#include "contiki.h"
#include "loader/elfloader.h"







PROCESS(test_codeload, "code load");
AUTOSTART_PROCESSES(&test_codeload);






/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_codeload, ev, data)
{
  int fd;

  PROCESS_BEGIN();

  /* Kill any old processes. */
  if(elfloader_autostart_processes != NULL) 
  {
    autostart_exit(elfloader_autostart_processes);
  }


  fd = cfs_open("blink.ce", CFS_READ | CFS_WRITE);
  if(fd < 0) 
  {
    printf("file error\n");
  } else 
  {
    int ret;
    char *print, *symbol;

    ret = elfloader_load(fd);
    cfs_close(fd);
    symbol = "";

    switch(ret) {
    case ELFLOADER_OK:
      print = "OK";
      break;
    case ELFLOADER_BAD_ELF_HEADER:
      print = "Bad ELF header";
      break;
    case ELFLOADER_NO_SYMTAB:
      print = "No symbol table";
      break;
    case ELFLOADER_NO_STRTAB:
      print = "No string table";
      break;
    case ELFLOADER_NO_TEXT:
      print = "No text segment";
      break;
    case ELFLOADER_SYMBOL_NOT_FOUND:
      print = "Symbol not found: ";
      symbol = elfloader_unknown;
      break;
    case ELFLOADER_SEGMENT_NOT_FOUND:
      print = "Segment not found: ";
      symbol = elfloader_unknown;
      break;
    case ELFLOADER_NO_STARTPOINT:
      print = "No starting point";
      break;
    default:
      print = "Unknown return code from the ELF loader (internal bug)";
      break;
    }
    printf("%s %s",print,symbol);

    if(ret == ELFLOADER_OK) 
    {
      int i;
      for(i = 0; elfloader_autostart_processes[i] != NULL; ++i) 
      {
    printf("exec: starting process %s\n",elfloader_autostart_processes[i]->name);
      }
      autostart_start(elfloader_autostart_processes);
    }

  }


  PROCESS_END();
}






