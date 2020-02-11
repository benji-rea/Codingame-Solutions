using System;

class Player {

  static void Main(string[] args) {

    string[] inputs = Console.ReadLine().Split(' ');
    int W = int.Parse(inputs[0]); // width of the building.
    int H = int.Parse(inputs[1]); // height of the building.
    int N = int.Parse(Console.ReadLine()); // maximum number of turns before game over.
    inputs = Console.ReadLine().Split(' ');
    int x = int.Parse(inputs[0]);
    int y = int.Parse(inputs[1]);

    int x1 = 0;
    int y1 = 0;
    int x2 = W - 1;
    int y2 = H - 1;

    while (true) {

      string BOMB_DIR = Console.ReadLine(); // the direction of the bombs from batman's current location (U, UR, R, DR, D, DL, L or UL)

      if (BOMB_DIR.Contains("U")) {
        y2 = y - 1;
      } else if (BOMB_DIR.Contains("D")) {
        y1 = y + 1;
      }

      if (BOMB_DIR.Contains("L")) {
        x2 = x - 1;
      } else if (BOMB_DIR.Contains("R")) {
        x1 = x + 1;
      }

      x = x1 + (x2 - x1) / 2;
      y = y1 + (y2 - y1) / 2;

      Console.WriteLine(x + " " + y);
    }
  }
}
