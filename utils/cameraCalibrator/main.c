#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#define TRUE 1
#define FALSE 0
struct v4l2_queryctrl queryctrl;
struct v4l2_querymenu querymenu;
int fd;

static void change_input(void)
{
  int index;

  index = 10;

  if (-1 == ioctl (fd, VIDIOC_S_INPUT, &index)) {
    perror ("VIDIOC_S_INPUT");
    exit (EXIT_FAILURE);
  }
}

static void print_input(void)
{
  struct v4l2_input input;
  int index;

  if (-1 == ioctl (fd, VIDIOC_G_INPUT, &index)) {
    perror ("VIDIOC_G_INPUT");
    exit (EXIT_FAILURE);
  }

  memset (&input, 0, sizeof (input));
  input.index = index;

  if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input)) {
    perror ("VIDIOC_ENUMINPUT");
    exit (EXIT_FAILURE);
  }

  printf ("Current input: %s\n", input.name);
}

/**
 * Enumerates possible values for limited controls
 * eg valid frame sizes
 */
static void enumerate_menu (void)
{
  printf ("  Menu items:\n");

  memset (&querymenu, 0, sizeof (querymenu));
  querymenu.id = queryctrl.id;

  for (querymenu.index = queryctrl.minimum;
       querymenu.index <= queryctrl.maximum;
       querymenu.index++) {
    if (0 == ioctl (fd, VIDIOC_QUERYMENU, &querymenu)) {
      printf ("  %s\n", querymenu.name);
    } else {
      perror ("VIDIOC_QUERYMENU");
      exit (EXIT_FAILURE);
    }
  }
}

/**
 * Reads current controls values
 * Prints current values as well as min and max of each control
 */
void read_controls(void)
{
  __u32 id;

  memset (&queryctrl, 0, sizeof (queryctrl));

  for (id = V4L2_CID_BASE;
       // +32 to catch 'hidden' controls
       id < V4L2_CID_LASTP1 + 32;
       id++) {
    queryctrl.id = id;
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      //Can we control this?
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
        continue;
      }

      printf ("%2d Control %22s", queryctrl.id - V4L2_CID_BASE,
              queryctrl.name);
      struct v4l2_control control;
      control.id = queryctrl.id;
      if(id != V4L2_CID_BASE + 33){ //causes a SIGKILL
         //Get the current value
         if (0 == ioctl (fd, VIDIOC_G_CTRL, &control)) {
            printf ("% 9d% 9d% 9d\n",
                    queryctrl.minimum, control.value, queryctrl.maximum);
         } else if (errno != EINVAL) {
            perror ("VIDIOC_G_CTRL - the Nao said we could control this, but "
                    "it won't tell us what the current value is.");
            exit (EXIT_FAILURE);
         }
      }
      else
         printf ("\n");
      if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
         enumerate_menu ();
    } else {
       if (errno == EINVAL)
          continue;

      perror ("VIDIOC_QUERYCTRL");
      exit (EXIT_FAILURE);
    }
  }


/*
  for (queryctrl.id = V4L2_CID_PRIVATE_BASE;; ++queryctrl.id) {
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;

      printf ("%d Control %s\n", queryctrl.id - V4L2_CID_PRIVATE_BASE,
              queryctrl.name);

      if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
        enumerate_menu ();
    } else {
      if (errno == EINVAL)
         break;

      perror ("VIDIOC_QUERYCTRL");
      exit (EXIT_FAILURE);
    }
  }

  for (queryctrl.id = V4L2_CID_CAMERA_CLASS_BASE;; ++queryctrl.id) {
    printf("queryctrl.id = %d\n", queryctrl.id);
    if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)
        continue;

      printf ("%d Control %s\n", queryctrl.id - V4L2_CID_CAMERA_CLASS_BASE,
              queryctrl.name);

      if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
        enumerate_menu ();
      break;
    } else {
      if (errno == EINVAL)
         continue;

      perror ("VIDIOC_QUERYCTRL");
      exit (EXIT_FAILURE);
    }
  }
*/
}

/**
 * Turns off auto adjust of controls: exposure, white balance, gain
 */
void disable_autos(void){
  struct v4l2_control control;
  memset (&control, 0, sizeof (control));

  control.id = V4L2_CID_AUTO_WHITE_BALANCE;//AWB Disable
  control.value = FALSE; /* silence */
  /* Errors ignored */
  if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
      && errno != ERANGE) {
    printf("exiting on AWB\n");
    perror ("VIDIOC_S_CTRL");
    exit (EXIT_FAILURE);
  }

  control.id = 0x009a0901;
  control.value = 0x0;
  if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
      && errno != ERANGE) {
    printf("exiting on AWB\n");
    perror ("VIDIOC_S_CTRL");
    exit (EXIT_FAILURE);
  }
/*
  control.id = V4L2_CID_BASE+32;//AEC Disable
  control.value = FALSE;
  if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
      && errno != ERANGE) {
    printf("exiting on AEC\n");
    perror ("VIDIOC_S_CTRL");
    exit (EXIT_FAILURE);
  }
*/
/*
  control.id = V4L2_CID_AUTOGAIN;//AGC Disable
  control.value = FALSE;  silence
//   Errors ignored 
  if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
      && errno != ERANGE) {
    printf("exiting on AGC\n");
    perror ("VIDIOC_S_CTRL");
    exit (EXIT_FAILURE);
  }
*/
}

/**
 * Gets name of control, and value to set it to
 * Changes the value of control to given value.
 */
void change_controls(void)
{
  printf("chooose a control to change: ");
  scanf("%d", &queryctrl.id);
  queryctrl.id += V4L2_CID_BASE;
  if (0 == ioctl (fd, VIDIOC_QUERYCTRL, &queryctrl)) {
    //Can we control this?
    if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED){
      printf("not valid!\n");
      return;
    }

    printf ("% 2d Control %20s", queryctrl.id - V4L2_CID_BASE,
            queryctrl.name);
    struct v4l2_control control;
    memset (&control, 0, sizeof (control));
    control.id = queryctrl.id;
    //Get the current value
    if (0 == ioctl (fd, VIDIOC_G_CTRL, &control)) {
      printf ("% 9d% 9d% 9d\n",
              queryctrl.minimum, control.value, queryctrl.maximum);
    } else if (errno != EINVAL) {
      perror ("VIDIOC_G_CTRL - the Nao said we could control this, but it "
	      "won't tell us what the current value is.");
      exit (EXIT_FAILURE);
    }
    if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
      enumerate_menu ();

    printf("value to set to: %d-%d: ", queryctrl.minimum, queryctrl.maximum);
    scanf("%d", &control.value);
    /* Errors ignored */
    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &control)
        && errno != ERANGE) {
      perror ("VIDIOC_S_CTRL");
      exit (EXIT_FAILURE);
    }
  } else {
    if (errno == EINVAL){
      printf("not valid!\n");
      return;
    }
    perror ("VIDIOC_QUERYCTRL");
    exit (EXIT_FAILURE);
  }
}

/**
 * Generally, prints current value, and possible range of values for each
 * control then provides option to change the current value of controls.
 */
int main()
{
  fd = open("/dev/video0", O_RDWR);
  if(fd < 0){
    perror("/dev/video1");
    exit (EXIT_FAILURE);
  }
  print_input();
  read_controls();
  //disable_autos();
  //change_input();
  fd = open("/dev/video1", O_RDWR);
  print_input();
  read_controls();
  change_controls();

  while(1){
    read_controls();
    change_controls();
  }

  return 0;
}
