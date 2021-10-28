// movimentação do robô

void movectrl(int dir1){
  switch(dir):{
    case 1: //frente
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, pwmmov);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, pwmmov);
    break;
    case 2: //virar a direita
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, pwmmov);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, pwmmov);
    analogWrite(motor2b, 0);
    break;
    case 3: //virar a direita
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, pwmmov);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, pwmmov);
    break;
    case 4: //para trás
    percentual = 70;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, pwmmov);
    analogWrite(motor2a, pwmmov);
    analogWrite(motor2b, 0);
    break;
    default: //parado
    percentual = 0;
    pwmmov = (percentual/100)*255;
    analogWrite(motor1a, 0);
    analogWrite(motor1b, 0);
    analogWrite(motor2a, 0);
    analogWrite(motor2b, 0);
    break;
  }
}

// movimentação bandeja
void platectrl(int dir2){
  switch(dir2):{
    case 1: //subindo
    analogWrite(motor3a, 1);
    analogWrite(motor3b, 0);
    break;
    case 2: //descendo
    analogWrite(motor3a, 0);
    analogWrite(motor3b, 1);
    break;
    default: //parado
    analogWrite(motor3a, 0);
    analogWrite(motor3b, 0);
    break;
  }
}
