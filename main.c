#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

int main() {
    printf("Hello, World!\n");
    
    struct passwd *pw = getpwuid(getuid());
    if (pw) {
        printf("Username: %s\n", pw->pw_name);
    } else {
        perror("getpwuid");
    }
    
    return 0;
}
