package tonecalibration;

import javax.sound.sampled.*;
import static java.lang.Math.*;

public class Main {
    public static void main(String[] args) throws LineUnavailableException {
        float sampleRate = 44100.0f/2;
        AudioFormat af = new AudioFormat(sampleRate,8, 1, true, false);
        SourceDataLine sdl = AudioSystem.getSourceDataLine(af);
        sdl.open(af);
        sdl.start();

        /*
        for (int wavelength=2; wavelength<200; wavelength++)
        {
            byte[] buffer = new byte[wavelength];
            for (int i=0; i<wavelength; i++)
                buffer[i] = (byte)(128*sin(i*PI*2/wavelength));

            double limit = 100./wavelength;
            int i=0;
            for (int j=0; j<1000; j++) sdl.write(buffer,i=(i+1)%wavelength,1);
        }*/
        /*
        for (int k=0; k<50000; k++)
        {
            double phase = 0.;
            byte[] buffer = new byte[1];
            for (double phi=0; phi<2*PI; phi+=0.0005)
            {
                phase += exp((cos(phi)+1)*0.5*log(0.5*PI));
                buffer[0] = (byte)(120*sin(phase));
                sdl.write(buffer,0,1);
            }
        }
        */
        
        for (int k=0; k<500; k++)
        {
            double phase = 0.;
            double speed = 0.08;
            byte[] buffer = new byte[1];
            for (int j=0; j<35000; j++)
            {
                buffer[0] = (byte)(128*sin(phase));
                sdl.write(buffer,0,1);
                phase += speed;
                speed *= 1.00012;
            }
            for (int j=0; j<35000; j++)
            {
                buffer[0] = (byte)(128*sin(phase));
                sdl.write(buffer,0,1);
                phase -= speed;
                speed /= 1.00012;
            }
        }
        sdl.drain();
        sdl.stop();
        sdl.close();
        System.out.println("Done");
    }
}
