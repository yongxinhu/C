//t0 = i, t1 = j, t2 = k, t4 = x, t5 = y, t6 = j
void mipstery(int *a0,int *a1,int *a2,int n) {
    for(int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
        	for(int k = 0;k < n;k++) {
		        int x = a2[n * i + j];
		        int y = a0[n * i + k];
		        int z = a1[n * k + j];
		        a2[n * i + j] = x + y * z;
	    	}
    	}
    }
}





