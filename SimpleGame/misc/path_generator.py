
import numpy as np
import matplotlib.pyplot as plt

#Parametric plot of
# (64*(cos(x)+1), 25*(arccos(cos(x)*sin(x)))+ 32)

PLOT_PATH = True
NUM_OF_SAMPLES = 50

def main():

    t = np.linspace(-np.pi, np.pi, NUM_OF_SAMPLES)
    x = 64*(np.cos(t) + 1)
    y = 25*np.arccos(np.cos(t)*np.sin(t)) - 25

    x_int = np.round(x, decimals=0).astype('uint8')
    y_int = np.round(y, decimals=0).astype('uint8')

    if(PLOT_PATH):
        plt.plot(x_int, y_int)
        plt.gca().invert_yaxis()
        plt.show()

    x_array_str = '{'
    y_array_str = '{'
    for i in range(x.size):
        if (i != (x.size-1)):
            x_array_str += str(x_int[i]) + ', '
            y_array_str += str(y_int[i]) + ', '
        
    x_array_str += str(x_int[x.size - 1]) + '};'
    y_array_str += str(y_int[x.size - 1]) + '};'
    

    X_POS_ARRAY_NAME = "static int const x_enemy_pos[] = " + x_array_str
    Y_POS_ARRAY_NAME = "static int const y_enemy_pos[] = " + y_array_str

    SAVE_FILE = open(r"./misc/generated_path.h", "w")

    SAVE_FILE.write(X_POS_ARRAY_NAME + '\n')
    SAVE_FILE.write(Y_POS_ARRAY_NAME + '\n')

    SAVE_FILE.close()


if __name__ == "__main__":
    main()