import java.math.BigInteger;
import static java.math.BigInteger.*;

public class Test {
    static class O {
        int a;

        O(int a) {
            this.a = a;
        }

        O add(O o) {
            return new O(a + o.a);
        }
    }
    public static void main(String args[]) {
        {
            String a = "a", b = "b",
                c = a + b;
            System.out.println(c);
        }
        {
            BigInteger a = ONE, b = TEN, c = valueOf(2),
                d1 = a.add(c.multiply(b).subtract(b.divide(c))),
                d2 = a + c*b - b/c;
            System.out.println(d1);
            System.out.println(d2);
        }
        {
            O o1 = new O(1), o2 = new O(2),
              o3 = o1 + o2;
           System.out.println(o3.a);
        }
    }
}
