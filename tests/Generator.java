public class Generator {

	public static void main(String[] args) {
		/* Recebe da linha de comando--------------------------*/
		/* 1. O numero de processos							   */
		/* 2. O limite de intervalo de chegada entre processos */
		/* 3. O limite dt de tempo para os processos           */
		/* 4. O lower bound (lb) onde deadline >= t0 + dt + lb */

		int n = Integer.parseInt(args[0]); 
		double interval = Double.parseDouble(args[1]);
		double limit    = Double.parseDouble(args[2]);
		double lb       = Double.parseDouble(args[3]);
		double prev_t0 = 0.0;
		
		for (int i = 0; i < n; i++) {
			double t0 = prev_t0 + (Math.random() * interval);
			double dt = Math.random() * limit;
			double deadline = t0 + dt + lb + (Math.random() * 4 * dt);
			System.out.format("%.3f processo%d %.3f %.3f%n", t0, i, dt, deadline);
			prev_t0 = t0;	
		}
	}
}