int main(int argc, char *argv[]){
    int arr[10];
    for(int i = 0; i < 11; i++){ // buffer overflow access
        arr[i] = i;
    }

    return 0;
}