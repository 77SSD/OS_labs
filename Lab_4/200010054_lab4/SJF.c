
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

struct process
{
    int pid;
    int burstTime[9999];
    int arrivalTime;
    bool done;
    bool inReady;
    int curBurst;
    float responseTime;
    float TAT;
    float totalCPU;
    float totalIO;
};

struct ready
{
    struct process proc;
    struct ready *next;
};

struct io
{
    struct process ioproc;
    struct io *nextio;
};

void sortedInsert(struct ready **head_ref, struct process *node)
{
    struct ready *new_node = (struct ready *)malloc(sizeof(struct ready));
    struct ready *current;
    new_node->proc = *node;
    if (*head_ref == NULL || (*head_ref)->proc.burstTime[(*head_ref)->proc.curBurst] > new_node->proc.burstTime[new_node->proc.curBurst])
    {
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
    else
    {
        current = *head_ref;
        while (current->next != NULL && current->next->proc.burstTime[current->next->proc.curBurst] <= new_node->proc.burstTime[new_node->proc.curBurst])
            current = current->next;
        new_node->next = current->next;
        current->next = new_node;
    }
}

void append(struct io **head_ref, struct process *node)
{
    struct io *new_node = (struct io *)malloc(sizeof(struct io));
    struct io *last = *head_ref;
    new_node->ioproc = *node;
    new_node->nextio = NULL;
    if (*head_ref == NULL)
    {
        *head_ref = new_node;
        return;
    }
    while (last->nextio != NULL)
        last = last->nextio;
    last->nextio = new_node;
    return;
}

int main(int argc, char *argv[])
{
    printf("ok");
    char ch, num[10];
    int linesCount = 0, burstCount = 0;
    FILE *fp = fopen(argv[1], "r");
    fseek(fp, 20, SEEK_SET);
    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '<')
            break;
        if (ch == '\n')
            linesCount++;
    }
    struct process ps[linesCount];
    fseek(fp, 20, SEEK_SET);
    for (int i = 0; i < linesCount; i++)
    {
        fscanf(fp, "%s", num);
        ps[i].pid = i;
        ps[i].done = false;
        ps[i].inReady = false;
        ps[i].arrivalTime = atoi(num);
        ps[i].curBurst = 0;
        ps[i].totalCPU = 0;
        ps[i].totalIO = 0;
        int j = 0;
        while (1)
        {
            fscanf(fp, "%s", num);
            ps[i].burstTime[j++] = atoi(num);
            burstCount += 1;
            if (atoi(num) == -1)
                break;
            else
            {
                if (j % 2 != 0)
                    ps[i].totalCPU += atoi(num);
                else
                    ps[i].totalIO += atoi(num);
            }
        }
    }
    fclose(fp);
    burstCount = burstCount / 2;
    int current_time = 0, completed = 0, ioburstTime = 0;
    bool running = false, runningio = false;
    int burstTime = INT_MAX, minimum = INT_MAX, pidio = INT_MAX;
    struct ready *start = NULL, *temp;
    struct io *startio = NULL, *junk;
    while (completed != burstCount)
    {
        for (int i = 0; i < linesCount; i++)
        {
            if (ps[i].arrivalTime <= current_time && ps[i].done == false && ps[i].inReady == false)
            {
                ps[i].inReady = true;
                sortedInsert(&start, &ps[i]);
            }
        }
        if (!running)
        {
            if (start != NULL)
            {
                minimum = start->proc.pid;
                running = true;
                burstTime = ps[minimum].burstTime[ps[minimum].curBurst];
                if (ps[minimum].curBurst == 0)
                    ps[minimum].responseTime = current_time - ps[minimum].arrivalTime;
                ps[minimum].curBurst += 1;
            }
            else
                minimum = INT_MAX;
        }
        if (running)
        {
            temp = start;
            start = start->next;
            temp = NULL;
            ps[minimum].inReady = false;
            completed += 1;
            running = false;
            current_time += burstTime;
            ps[minimum].done = true;
            ps[minimum].TAT = current_time - ps[minimum].arrivalTime;
        }
        else
        {
            current_time += 1;
            burstTime = 1;
        }
        int time = burstTime;
        while (time > 0 && startio != NULL)
        {
            if (!runningio)
            {
                pidio = startio->ioproc.pid;
                runningio = true;
                ioburstTime = ps[pidio].burstTime[ps[pidio].curBurst];
                ps[pidio].curBurst += 1;
            }
            else
            {
                if (ioburstTime > burstTime)
                {
                    ioburstTime = ioburstTime - burstTime;
                    time = 0;
                }
                else
                {
                    junk = startio;
                    startio = startio->nextio;
                    junk = NULL;
                    runningio = false;
                    ps[pidio].done = false;
                    time = time - ioburstTime;
                }
                if (startio == NULL)
                    break;
            }
        }
        if (minimum != INT_MAX)
        {
            if (ps[minimum].burstTime[ps[minimum].curBurst] != -1)
                append(&startio, &ps[minimum]);
        }
    }
    FILE *fpt;
    fpt = fopen("File.csv", "w+");
    float averageTAT = 0;
    float averagePenaltyRatio = 0;
    float averageWaiting = 0;
    fprintf(fpt, "PID, TurnAround Time, Penalty Ratio, Waiting Time\n");
    printf("PID, TurnAround Time, Penalty Ratio, Waiting Time\n");
    for (int i = 0; i < linesCount; i++)
    {
        averageTAT += ps[i].TAT;
        averagePenaltyRatio += (ps[i].TAT / ps[i].totalCPU);
        averageWaiting += (ps[i].TAT - ps[i].totalCPU);
        fprintf(fpt, "%d, %f, %f, %f\n", ps[i].pid, ps[i].TAT, (ps[i].TAT / ps[i].totalCPU), (ps[i].TAT - ps[i].totalCPU));
        printf("%d, %f, %f, %f\n", ps[i].pid, ps[i].TAT, (ps[i].TAT / ps[i].totalCPU), (ps[i].TAT - ps[i].totalCPU));
    }
    averageTAT = averageTAT / linesCount;
    averagePenaltyRatio = averagePenaltyRatio / linesCount;
    averageWaiting = averageWaiting / linesCount;
    printf("\nAverage TurnAround Time : %f\n", averageTAT);
    printf("\nAverage penalty Ratio : %f\n", averagePenaltyRatio);
    printf("\nAverage Waiting Time : %f\n", averageWaiting);
    printf("\nSystem Throughput : %f\n", ((float)linesCount / (float)current_time)); // Number of processes completed per unit time
}