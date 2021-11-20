void transpose(){
  transpose(0,0,N,N);
}

void transpose(unsigned x, unsigned y, unsigned x_size, unsigned y_size){
  if ((x_size == 0) || (y_size == 0))
    return;
  if ((x_size == 1) && (y_size == 1)){
     if (x < y)
       swap(x,y,y,x);
  }
  else
  {
      int half_x_size = (x_size + 1) / 2;
      int half_y_size = (y_size + 1) / 2;
      transpose(x, y, half_x_size, half_y_size);
      transpose(x+ half_x_size, y+half_y_size, x_size- half_x_size, y_size- half_y_size);
      transpose_and_swap(x, y + half_y_size, half_x_size, y_size - half_y_size);
  }
}

void transpose_and_swap(unsigned x, unsigned y, unsigned x_size, unsigned y_size )
{

  if ((x_size == 0) || (y_size == 0))
      return;
  if ((x_size == 1) && (y_size == 1))
  {
    if (x < y)
        {
        swap(x,y,y,x);
        }

  }
  else
  {
    int half_x_size = (x_size + 1) / 2;
    int half_y_size = (y_size + 1) / 2;
    transpose_and_swap(x, y, half_x_size, half_y_size);
    transpose_and_swap(x, y + half_y_size, half_x_size, y_size - half_y_size);
    transpose_and_swap(x + half_x_size, y, x_size - half_x_size, half_y_size);
    transpose_and_swap(x + half_x_size, y + half_y_size, x_size - half_x_size, y_size - half_y_size);
    }
}

