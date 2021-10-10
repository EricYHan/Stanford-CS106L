#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include "SimpleGraph.h"

using namespace std;

const double PI = 3.14159265358979323;
const double kRepel = 0.001;
const double kAttract = 0.001;

struct nodeForce {
    double deltaX;
    double deltaY;
};

void Welcome();
void initNodePosition(int number, SimpleGraph & graph);
void computeForce(SimpleGraph & graph);
int getGraph(string name, SimpleGraph & graph);
int getInteger(const string &prompt = "Enter the time(s) you want to run the algorithm:",
               const string &reprompt = "Illegal numeric format. Try again.\n");

// Main method
int main() {
    Welcome();

    int nodeNumber;
    int runTime;

    while(true) {
        SimpleGraph inputGraph;
        string inputName;
        ifstream graphFile;

        cout << "Please input the name of graph: " << endl;
        getline(cin, inputName);
        cout << "The name you input is: " << inputName << endl;

        nodeNumber = getGraph(inputName, inputGraph);
        if (nodeNumber == -1) {
            continue;
        }
        runTime = getInteger();
        initNodePosition(nodeNumber, inputGraph);
        InitGraphVisualizer(inputGraph);
        DrawGraph(inputGraph);
        time_t startTime = time(NULL);
        cout << "The algorithm is running, Please wait..." << endl;
        while(true) {
            if (difftime(time(NULL), startTime) == runTime) {
                break;
            }
            computeForce(inputGraph);
            DrawGraph(inputGraph);
        }
    }
    return 0;
}

void computeForce(SimpleGraph & graph)
{
    vector<nodeForce> force(graph.nodes.size(), {0, 0});
    for (size_t i = 0; i < graph.edges.size(); i++) {
        double x0 = graph.nodes.at(graph.edges.at(i).start).x;
        double y0 = graph.nodes.at(graph.edges.at(i).start).y;
        double x1 = graph.nodes.at(graph.edges.at(i).end).x;
        double y1 = graph.nodes.at(graph.edges.at(i).end).y;
        double fAttract = kAttract * (pow(y1 - y0, 2) + pow(x1 - x0, 2));
        double theta = atan2(y1 - y0, x1 - x0);
        force[graph.edges[i].start].deltaX += fAttract * cos(theta);
        force[graph.edges[i].start].deltaY += fAttract * sin(theta);
        force[graph.edges[i].end].deltaX -= fAttract * cos(theta);
        force[graph.edges[i].end].deltaY -= fAttract * sin(theta);
    }
    for (size_t i = 0; i < graph.nodes.size() - 1; i++) {
        for (size_t j = i + 1; j < graph.nodes.size(); j++) {
            double x0 = graph.nodes[i].x;
            double y0 = graph.nodes[i].y;
            double x1 = graph.nodes[j].x;
            double y1 = graph.nodes[j].y;

            double fRepel = kRepel / sqrt(pow(y1 - y0, 2) + pow(x1 - x0, 2));
            double theta = atan2(y1 - y0, x1 - x0);
            force[i].deltaX -= fRepel * cos(theta);
            force[i].deltaY -= fRepel * sin(theta);
            force[j].deltaX += fRepel * cos(theta);
            force[j].deltaY += fRepel * sin(theta);
        }
    }
    for (size_t k = 0; k < graph.nodes.size(); k++) {
        graph.nodes.at(k).x += force.at(k).deltaX;
        graph.nodes.at(k).y += force.at(k).deltaY;
    }
}

int getGraph(string name, SimpleGraph & graph)
{
    ifstream graphFile;
    Edge tempEdge;
    int number;

    graphFile.open(name);
    if (!graphFile.is_open()) {
        cout << "The graph name you input is wrong" << endl;
        return -1;
    }
    graphFile >> number;
    cout << "Total nodes is " << number << endl;
    while (graphFile >> tempEdge.start) {
        graphFile >> tempEdge.end;
        graph.edges.push_back(tempEdge);
    }
    graphFile.close();
    return number;
}

void initNodePosition(int number, SimpleGraph & graph)
{
    for (int i = 0; i < number; i++) {
        Node tempNode;
        tempNode.x = cos(2 * PI * (i + 1) / number);
        tempNode.y = sin(2 * PI * (i + 1) / number);
        graph.nodes.push_back(tempNode);
    }
}

int getInteger(const string &prompt, const string &reprompt)
{
    while (true)
    {
        cout << prompt;
        string line;
        if (!getline(cin, line))
        {
            throw domain_error("...");
        }

        istringstream iss(line);
        int val;
        char remain;

        /*iss转变成int可以识别负数*/
        if (iss >> val && !(iss >> remain))
        {
            return val;
        }
        cout << reprompt;
    }
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome()
{
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}
