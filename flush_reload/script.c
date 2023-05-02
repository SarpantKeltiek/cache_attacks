#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define LOOP 5
#define LOOP_MOY 10

static int temps[LOOP] = {100000, 150000, 200000, 250000, 300000};
static int fauxpos[LOOP_MOY];
static float moy[LOOP];
int i = 0;

int main() {
    for(int j=0; j<2; j++){
        for(int k=0; k<1; k++){
            // nom des deux exécutables à lancer
            char* exec1 = "./victim";
            char* exec2 = "./flushreload";

            // commandes pour lancer les exécutables
            char* cmd2[] = {exec2, NULL};

            char arg1[50];
            sprintf(arg1, "%d", temps[j]); // convertit l'entier en chaîne de caractères
            char* cmd1[] = {exec1, arg1, NULL};

            // lancement des exécutables avec fork
            pid_t pid1, pid2;
            pid1 = fork();
            if (pid1 == 0) {
                FILE* out1 = freopen("vict", "w", stdout);
                execvp(exec1, cmd1);
                exit(0);
            } else {
                pid2 = fork();
                if (pid2 == 0) {
                    FILE* out2 = freopen("att", "w", stdout);
                    execvp(exec2, cmd2);
                    exit(0);
                }
            }

            // attente de 10 secondes
            sleep(10);

            kill(pid1, SIGINT); // envoie le signal SIGINT à victim
            kill(pid2, SIGINT); // envoie le signal SIGINT à FR

            // récupération de la sortie des exécutables
            char buffer[BUFFER_SIZE];
            FILE* fp;
            int nb1, nb2;
            fp = popen("tail -1 vict", "r");
            fgets(buffer, BUFFER_SIZE, fp);
            sscanf(buffer, "Nb of access: %d", &nb1);
            pclose(fp);

            fp = popen("tail -1 att", "r");
            fgets(buffer, BUFFER_SIZE, fp);
            sscanf(buffer, "Nb of access: %d", &nb2);
            pclose(fp);

            printf("\n\nPour j = %d et temps = %d\n", j, temps[j]);

            printf("Nombre d'accès de victim : %d\n", nb1);
            printf("Nombre d'accès de flush : %d\n", nb2);

            int faux_positif = abs(nb2-nb1);

            printf("Nombre de faux positifs : %d\n", faux_positif);

            fauxpos[k] = faux_positif;
        }

            float somme = 0.0;
            for(int i=0; i<LOOP_MOY; i++) {
                somme += fauxpos[i];
            }
            
            moy[j] = somme/10;
    }

    for (int a = 0; a<LOOP; a++){
        printf("moy[%d] = %f\n", a, moy[a]);
    }

    system("rm vict");
    system("rm att");

    // Ouverture du processus gnuplot en mode "écriture"
    FILE *plot = popen("gnuplot -persist", "w");

    // Vérification de l'ouverture du processus
    if (!plot) {
        perror("Erreur d'ouverture du processus gnuplot");
        exit(EXIT_FAILURE);
    }

    // Envoi des commandes gnuplot
    fprintf(plot, "set title \"Moyenne en fonction du temps d'attente\"\n");
    fprintf(plot, "set xlabel \"Temps d'attente\"\n");
    fprintf(plot, "set ylabel \"Moyenne des faux positifs\"\n");
    fprintf(plot, "plot '-' with linespoints pointtype 2 pointsize 1.5 title \"Moyenne en fonction du temps d'attente\"\n");

    // Envoi des données à tracer
    for (int i = 0; i < LOOP; i++) {
        fprintf(plot, "%d %f\n", temps[i], moy[i]);
    }

    // Envoi de la commande de fin de données
    fprintf(plot, "e\n");

    // Fermeture du processus gnuplot
    pclose(plot);
    return 0;
}
