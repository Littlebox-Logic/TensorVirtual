// Tensor VM - helpinfo.c

char *helpinfo = "\
Usage:\n\
\t--help\t\t: Display HELP info.\n\
\t--version\t: Get the VERSION info.\n\
\t--quiet\t\t: Disable LOG output.\n";

void show_help(void)
{
	printf("HELP info:\n\
\t\033[;97mreg\033[0m\t: Display VM Registers' values.\n\
\t\033[;97mboot\033[0m\t: Load bootable image and boot virtual machine.\n\
\t\033[;97mc\033[0m\t: Continue to run until meet next pause point.\n\
\t\033[;97minstr\033[0m\t: Display the pointer value and instruction.\n\
\t\033[;97mclear\033[0m\t: Clear screen.\n\
\t\033[;97mhelp\033[0m\t: Display this HELP info.\n");
}
