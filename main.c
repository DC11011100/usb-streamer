//
// Created by 0xDC on 1/21/20.
//

/*
 * Usage: usb-streamer <bus> <device-id> <to-send.txt>
 *
 * to-send.txt is a newline separated file of bytes to send, starting from beginning
 */

#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>


typedef struct {
    int bus;
    int id;
    char *path;
} streamer_arg_t;

static void
print_devs(libusb_device **devs)
{
    libusb_device *dev;
    int i = 0, j = 0;
    uint8_t path[8];

    while ((dev = devs[i++]) != NULL) {
        struct libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            fprintf(stderr, "failed to get device descriptor");
            return;
        }

        printf("%04x:%04x (bus %d, device %d)",
               desc.idVendor, desc.idProduct,
               libusb_get_bus_number(dev), libusb_get_device_address(dev));

        r = libusb_get_port_numbers(dev, path, sizeof(path));
        if (r > 0) {
            printf(" path: %d", path[0]);
            for (j = 1; j < r; j++)
                printf(".%d", path[j]);
        }

        printf("\n");
    }
}

libusb_device *
find_device(libusb_device** device_list, int n_devices, int bus, int id)
{
    for (int i=0; i<n_devices; i++) {
        libusb_device *dev = device_list[i];
        if (bus == libusb_get_bus_number(dev) &&
            id  == libusb_get_device_address(dev)) {
            return dev;
        }
    }

    return NULL;
}

void
streamer_print_intro(streamer_arg_t *args)
{
    printf("\n\n"
           "Output Device:\n"
           "    Bus:%03d Device:%03d\n"
           "Data Stream:\n"
           "    '%s'\n\n",
           args->bus,
           args->id,
           args->path);
}

int
streamer_run_session(streamer_arg_t *args)
{
    streamer_print_intro(args);

    printf("Opening target device...");
    libusb_device **usb_devices;
    int n_devices = libusb_get_device_list(NULL, &usb_devices);
    libusb_device *out_device = find_device(usb_devices, n_devices, args->bus, args->id);
    // TODO: Actually bind to device, list available descriptors, prompt user to select one
    //       then start pumping data from infile
    if (out_device) {
        printf("SUCCESS\n");
    } else {
        printf("FAIL\n");
    }

    libusb_free_device_list(usb_devices, 1);
}



int
streamer_get_args(int argc, char **argv, streamer_arg_t *args)
{
    if (argc == 4) {
        args->bus  = strtol(argv[1], 0, 0);
        args->id   = strtol(argv[2], 0, 0);
        args->path = argv[3];
        return 0;
    } else {
        return -1;
    }
}


int
main(int argc, char **argv)
{
    streamer_arg_t args;
    if (streamer_get_args(argc, argv, &args) < 0) {
        return -1;
    }

    libusb_init(NULL);
    streamer_run_session(&args);
    libusb_exit(NULL);
    return 0;
}


