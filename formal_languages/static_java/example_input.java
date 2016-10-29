public class Gen {
    public static void main(String[] args) {
        int a;
        int b;
        readln(a);
        readln(b);
        println(nod(a, b));
        println(nonRecursiveNod(a, b));
    }

    static int nod(int a, int b) {
    if (a == 0 || b == 0) {
            return a + b;
        } else {
            if (a > b) {
                return nod(a%b, b);
            } else {
                return nod(a, b%a);
            }
        }
    }
    
    static int nonRecursiveNod(int a, int b) {
        int tmp;
        if (a < b) {
            tmp = a;
            a = b;
            b = tmp;
        }

        while (b > 0) {
            tmp = b;
            b = a % b;
            a = tmp;
        }

        return a;
    }
}
