import java.util.concurrent.*;
import java.util.Arrays;

public class FJ_Quick extends RecursiveAction {
    static final int SIZE = 10000;
    static final int THRESHOLD = 50;

    private int begin;
    private int end;
    private int[] array;

    public FJ_Quick(int begin, int end, int[] array) {
        this.begin = begin;
        this.end = end;
        this.array = array;
    }

    protected void compute() {
        if (end - begin < THRESHOLD) {
            // conquer stage 
            for (int i = begin+1; i <= end; i++){
                int p = array[i];
                int j = i-1;
                while(j>=begin && array[j]>p){
                    array[j+1] = array[j];
                    j--;
                }
                array[j+1] = p;
            }
        }
        else {
            // divide stage 
            int r = begin, s = end, pivot = array[begin];
            while (r < s)
            {
                while(r < s && array[s] >= pivot){
                    s--;
                }
                if(r < s)
                    array[r++] = array[s];

                while(r < s && array[r] < pivot){
                    r++;
                }
                if(r < s)
                    array[s--] = array[r];
            }
            array[r] = pivot;

            int mid = r;
            
            FJ_Quick leftTask = new FJ_Quick(begin, mid, array);
            FJ_Quick rightTask = new FJ_Quick(mid + 1, end, array);

            leftTask.fork();
            rightTask.fork();
            leftTask.join();
            rightTask.join();

            int[] temp = new int[SIZE];
            
            int i = begin,j = mid+1,k = begin;
            while(i<=mid && j<=end){
                if(array[i]<array[j])
                    temp[k++] = array[i++];
                else
                    temp[k++] = array[j++];
            }
            while(i<=mid)
                temp[k++] = array[i++];
            
            while(j<=end)
                temp[k++] = array[j++];
            
            int p = begin;
            //System.out.println(Arrays.toString(temp));
            while(p<=end){
                array[p] = temp[p];
                p++;
            }
        }
    }

	public static void main(String[] args) {
		ForkJoinPool pool = new ForkJoinPool();
		int[] array = new int[SIZE];

		// create SIZE random integers between 0 and 9
		java.util.Random rand = new java.util.Random();

		for (int i = 0; i < SIZE; i++) {
			array[i] = rand.nextInt(100);
		}		
		
		// use fork-join parallelism to sum the array
		FJ_Quick task = new FJ_Quick(0, SIZE-1, array);

		pool.invoke(task);
        
		System.out.println("The result is ");
        System.out.println(Arrays.toString(array));
	}
}
