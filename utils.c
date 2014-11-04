
void shortIntoBytesArray(short val,char *bytesArr){
	short *pointerToVal = &val;
	char *pointerValAsBytes = (char *)pointerToVal;//look at the short as an array of Bytes

	for (int i = 0; i < 2 ;i++){
		bytesArr[i]=pointerValAsBytes[i];
	}
}

short bytesArrayIntoShort(char *bytesArr){
	short *my=(short *)bytesArr;
	return *my;
}