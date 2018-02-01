#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <unistd.h>
#include <fcntl.h>



/* ========================================
 *
 * McKay Fenn,    u0939404
 * Assignment 1,  CS5460: Operating Systems
 *
 * ========================================
 */
 
 
/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign1.c 
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign1.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort (unsigned long arg)
{
  unsigned long result[8];
  
  int i, j;

  // first get the argument into result array
  for (i = 0; i < 8; i++) {
    // and current byte with 0xff
    result[i] = arg & (unsigned long)0xff;
    // then move to the next byte
    arg = arg >> 8;
  }
  

  // now do a simple selection sort algorithm
  for (i = 0; i < 8; i++) {
    int min = i;
    for (j = i; j < 8; j++) {
      if (result[j] < result[min]) {
        min = j;
      }
    }
    int temp = result[i];
    result[i] = result[min];
    result[min] = temp;
  }
  
  // and get result back into a good format
  unsigned long return_result = 0;
  for (i = 0; i < 8; i++) {
    return_result = return_result | (result[i] << 8*i);
  }


  return return_result;

}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
  /* -----------
   * This can be done very similarly to byte_sort, just need to change a few values 
   * -----------
   */
  unsigned long result[16];
  
  int i, j;

  // first get the argument into result array
  for (i = 0; i < 16; i++) {
    // and current byte with 0xf
    result[i] = arg & (unsigned long)0xf; // this is changed to 0xf
    // then move to the next byte
    arg = arg >> 4;
  }
  

  // now do a simple selection sort algorithm
  for (i = 0; i < 16; i++) {
    int min = i;
    for (j = i; j < 16; j++) {
      if (result[j] < result[min]) {
        min = j;
      }
    }
    int temp = result[i];
    result[i] = result[min];
    result[min] = temp;
  }
  
  // and get result back into a good format
  unsigned long return_result = 0;
  for (i = 0; i < 16; i++) {
    return_result = return_result | (result[i] << 4*i);
  }


  return return_result;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt *name_list (void)
{
  // my name and pointer to iterate
  const char *my_name = "mckay";
  const char *p = my_name;

  // create the head of the linked list
  struct elt* head = (struct elt*)malloc(sizeof(struct elt));
  // if any call to malloc() fails, free any heap memory and return NULL
  if (head == NULL) {
    free(head);
    return NULL;
  }

  // now set the value and next
  head->val = my_name[0];
  
  struct elt* current;
  struct elt* next = head; // used to keep track of which to point to 

  // need to start i at 1 since we've already created the head
  int i = 1;
  while (*p != '\0') {
    // allocate here 
    current = (struct elt*)malloc(sizeof(struct elt));

    // if any call to malloc() fails, free any heap memory return NULL
    if (current == NULL) {
      current = head;
      while (current != NULL) {
        free(current);
        current = current->link;
      }
      return NULL;
    }

    current->val = my_name[i];

    // now update the next 
    if (next != NULL)
      next->link = current;
    
    // and update
    next = current;

    p++;
    i++;
  }

  
  return head;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert (enum format_t mode, unsigned long value)
{
  // first check if mode is allowed
  if (mode == OCT || mode == BIN || mode == HEX) {

    // get base, number of bits, and length depending on what mode is
    int base = 0;
    int num_bits = 0;
    int length = 0;
    if (mode == BIN) {
      base = 2;
      num_bits = 1;
      length = 64;
    }
    else if (mode == OCT) {
      base = 8;
      num_bits = 3;
      length = 22;
    }
    else if (mode == HEX) {
      base = 16;
      num_bits = 4;
      length = 16;
    }


    unsigned long remainder = value;
    char result[length];

    int move_bits;
    int index = 0;

    // calculate values
    for (move_bits = 0; move_bits < 64; move_bits += num_bits) {
      

      // if this is HEX we need to deal with letters
      // so we mod to find which number it corresponds to
      // and if its a number we can deal with it like normal 
      // by (remainder % base) + '0'
      if (mode == HEX) {
        switch (remainder % base) {
          case 15: {
            result[index] = 'f';
            break;
          }
          case 14: {
            result[index] = 'e';
            break;
          }
          case 13: {
            result[index] = 'd';
            break;
          }
          case 12: {
            result[index] = 'c';
            break;
          }
          case 11: {
            result[index] = 'b';
            break;
          }
          case 10: {
            result[index] = 'a';
            break;
          }
          default: {
            result[index] = (remainder % base) + '0';
          }
        }
      }
      else {
        result[index] = (remainder % base) + '0';
      }

      // continue
      index++;
      remainder = remainder / base;
    }
    
    // now print out with putc
    int i;
    for (i = length - 1; i >= 0; i--) {
      putc(result[i], stdout);
    }
    putc('\n', stdout);


    return;
  }
  else {
    // print nothing and return if mode was not allowed
    return;
  }

}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me (void)
{

  // open the file with SYS_open
  // give it mode parameters to make it writable and executable
  int filedescriptor = syscall(SYS_open, "./me.txt", O_WRONLY | O_CREAT | O_EXCL , S_IRWXU | S_IRWXG | S_IRWXG);
  if (filedescriptor < 0) {
    // can get here if the file already exists
    return;
  }

  char ascii_me[] = "\n \
                    mmmmdhyyhyyysss-.................................------.................................... \n \
                    mmmmmdhyhhhhhhyho-..................................---------.............................. \n \
                    ddmmmhhyhddddddyho-....................................-------------....................... \n \
                    Nmmmmdhyshhhhhyhhys+-...................................---------------...----............. \n \
                    Nmmmmmhhyyhddmmdhhyso++oo/:-...........................--------------.-------.............. \n \
                    hysyhhyhdhhmNNNmmdhhhhhhhhys+:-........................---------.-......----............... \n \
                    hhyyhddddmmmmNNNmmddddhhhhhhhhy/................-...--------::::--.......--................ \n \
                    mdddhddddddddmmmmmdhyhdddddmmddhy/------.--..------://++osyyyhhhys+:--..-.................. \n \
                    mmmddddddddmmmddmmNmmddmmmmmmmmmmmh/--------:::++//+oossossssyyyhdmdhys/:.................. \n \
                    mmmmddhhhdddddmmNNMMMMNNNNNNNNmmmmmd+::::///////////+//oo///////+oymNmmdho-................ \n \
                    mmmmmdmmmmNNNNNNNNNNNdhhyhhhhhyysoo+///////////////oss/oy+////////+sdmNNmdo-............... \n \
                    NNNNNNNNNNNNNNNNNNNhsoooooooooo+//////////////////+shh+oho/////////+ymmmmmhs:.............. \n \
                    NNNNNNNNNNNNNNNNNds+++++++++++///////+o+/++/+++//++oys+oo+/////////+odmmddddo............-- \n \
                    NNNNNNNNNNNNNNNNh+/////////++/////////o+/++//+++++++o+/////////////+ohNmmdddd:..........-:- \n \
                    NNNNNNNNNNNNNNNh+//////////++/////++++so+++/+++o++++///////////////+shmNmdddd/.........--:: \n \
                    NNNNNNNNNNNNNNds+//////////o+++++++++osoo+o++oooooooo++//////////++oydmmmmhdd+.........---- \n \
                    mNNNNNNNNNNNNmho+//////++++o+++++++++oysoosoosssooossso+++///////+oshdmmmmddo-.........---- \n \
                    mmmmNNNNNNNNNmyo+//++++++oooooooo++++ossooooooooooosyso+yy++++++++oyhmmmmdhy-...........--- \n \
                    mmmmNNNNNNmmNmyo++++++++oooooooooo++++o++++++++oo++ohds+yho++++++oyhdmmdddy:..............- \n \
                    mmmmmmNNmmmmmNdsooooooooooooosssooooo+++++++oooo+++osyo+yyo++++osyddmdddhs:............---- \n \
                    NNNNNNNmmNmmmmNdyssssssssssssssssssssoooooooooooo++ossooyso+++oshdmmddhy/-............--.-- \n \
                    NmNNNNNNNNNNNNNNmdhyyyyyhhhhdddddddddd////+++ooooooooooososssyhdmmdhyo/-..............----- \n \
                    NNNNNNNNNNNNNNNNNNNmmddddmmmNNNNNNNNNy------:/++soosyyyhhhdddddhhyo:-.................----- \n \
                    NNNNNNNNNNNNNNNNNNNNNNNNmdhhhhhhhdmNh::-:::---::::::::::://///::-------..............------ \n \
                    mNmmmmmmmmddddddhhhddmdhyyyyhhdhhddy/::::----------------............................------ \n \
                    ddmmmmmmmmmmddhdhyyyyhddddhsyhhhhdy:-------------------..............................------ \n \
                    mmmmmmmmmdhhhhdddddmmmmmmho::oyhdy:-----------------................................------- \n \
                    ddddddddhhhddmmmmmmmmmmddo:---/ss:----------------...................................-.-..- \n \
                    hdddhhhddNmmdhmdmmmmdddhs:------------------------................................-........ \n \
                    dhhdhmNNNNmmddmmmmddhhhs:-------------------------..............................---.----.-- \n \
                    ddNNNNNNNNmmdhdhhhyyyhy:--------------------------..............................----------- \n \
                    mmNNNNNNNmmhhssyyhhhhy:-----------------------------..........................---.--------- \n \
                    ";
                    
    

    // try to write to the file
    // close and unlink it if it fails
    int write = syscall(SYS_write, filedescriptor, ascii_me, sizeof(ascii_me)-1);
    if (write < 0) {
      syscall(SYS_close, filedescriptor);
      syscall(SYS_unlink, "./me.txt");
      return;
    }


    // make sure we can close the file
    syscall(SYS_close, filedescriptor);
}

/*
int  main( int argc, char *argv[]) {
  // print byte_sort
  printf("byte sort: \n");
  unsigned long result = byte_sort(0x0403deadbeef0201);
  printf("%lx\n", result);
  

  // test nibble_sort
  printf("nibble sort: \n");
  result = nibble_sort(0x0403deadbeef0201);
  printf("%lx\n", result);


  // test name_list
  struct elt *name_list_result = name_list();
  int worked = 0;
  if (name_list_result == NULL) {
    printf("name_list result was NULL");
    worked = 1;
  }
  
  if (worked == 0) {
    while (name_list_result->link != NULL) {
      printf("%c", name_list_result->val);
      name_list_result = name_list_result->link;
    }
    printf("\n");
  }



  // test convert
  convert(HEX, 0xdeadbeef);
  convert(66, 18446744073709551615lu);
  convert(66, 0x492492);

  // test draw me
  draw_me();


  return 0;

}
*/
