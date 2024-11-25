//
// Created by peros on 2023.11.04.
//
/**
 * A segedfuggvenyeket es valtozokat tarolo file
 *
 * @author Eros Pal
 * @since 2023.11.04.
 */

#include <string>
#include <fstream>
#include <queue>
#include "auxiliary.h"
#include "OpenMesh/Core/IO/MeshIO.hh"

/**
 * Beolvassa a megadott filet
 * @param FILE a beolvasando file
 * @param mesh a mesh, amibe tarolni kell az adatokat
 * @since 1.1
 */
void readMesh(const std::string& FILE, MyMesh& mesh){
    if(!OpenMesh::IO::read_mesh(mesh, FILE)){
        std::cerr << "Error: Cannot read mesh from " << FILE << std::endl;
        exit(1);
    }
}

/**
 * A parameterkent kapott bemeneti es kimeneti pontokat osszekoti fuggolegesen es kiirja a .obj fileba
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @param INPUT_FILE_NAME a bemeneti file neve
 * @param intersect_points a metszespontok koordinatai
 * @param desc a leiras
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.1
 */
void writeInternalLines(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME,
                        std::vector<Point> &intersect_points, const std::string &desc, bool is_finish) {
    if (is_finish) {
        std::ofstream file(OUTPUT_FILE_NAME);
        if (!file) {
            std::cerr << "Error: The file " << OUTPUT_FILE_NAME << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << desc << INPUT_FILE_NAME << " by BTMLYV\n";
        int k = 1;
        for (int i = 0; i < (int) intersect_points.size(); i++) {
            file << "v " << intersect_points[i]._coordinates[0] << " " << intersect_points[i]._coordinates[1] << " "
                 << intersect_points[i]._coordinates[2] << "\n";
            i++;
            file << "v " << intersect_points[i]._coordinates[0] << " " << intersect_points[i]._coordinates[1] << " "
                 << intersect_points[i]._coordinates[2] << "\n";
            file << "l " << k << " " << k + 1 << "\n";
            k = k + 2;
        }
        file.close();
        writeLog("\tInternal lines written to file");
    }
}

/**
 * Kiszamolja a haromszog teruletet
 * @param x_1 a kerdeses pont x koordinataja
 * @param z_1 a kerdeses pont z koordinataja
 * @param x_2 az masodik pont x koordinataja
 * @param z_2 az masodik pont z koordinataja
 * @param x_3 az harmadik pont x koordinataja
 * @param z_3 az harmadik pont z koordinataja
 * @return a haromszog terulete
 * @since 1.1
 */
double area(double x_1, double z_1, double x_2, double z_2, double x_3, double z_3) {
    double v_1[2];
    v_1[0] = x_1 - x_2;
    v_1[1] = z_1 - z_2;

    double v_2[2];
    v_2[0] = x_1 - x_3;
    v_2[1] = z_1 - z_3;

    double A_2 = v_1[0] * v_2[1] - v_2[0] * v_1[1];
    return A_2 / 2;
}

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az x koordinata alapjan, majd az z, majd az y koordinata alapjan
 * @param p_1 az eslo pont
 * @param p_2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.1
 */
bool comparePoints(Point& p_1, Point& p_2) {
    /// A szamokat kerekitjuk hat tizedes jegyre
    for (int i = 0; i < 3; i++) {
        p_1._coordinates[i] = round(p_1._coordinates[i] * 1000000) / 1000000;
        p_2._coordinates[i] = round(p_2._coordinates[i] * 1000000) / 1000000;
    }

    if(p_2._coordinates[0] - p_1._coordinates[0] > p_1._e){
        return true;
    } else if(std::abs(p_1._coordinates[0] - p_2._coordinates[0]) <= p_1._e){
        if(p_2._coordinates[2] - p_1._coordinates[2] > p_1._e){
            return true;
        } else if(std::abs(p_1._coordinates[2] - p_2._coordinates[2]) <= p_1._e){
            if(p_2._coordinates[1] - p_1._coordinates[1] > p_1._e){
                return true;
            }
        }
    }

    return false;
}

/**
 * Visszaadja a ket pont kozotti vektor sulyat
 * @param ACTUAL_POINT aktualis pont
 * @param ADJACENT_POINT szomszedos pont
 * @param l a hibahatarhoz az osztas
 * @return 0 ha a ket pont kozti vektor szoge hatarerteken beluli, egyebkent a szog erteke lesz a suly, -1 ha alatta van
 * @since 1.2
 */
double thisEdgeLeadsToPoint(const Point &ACTUAL_POINT, const Point &ADJACENT_POINT, double l) {
    double e = l / 100;
    if(ACTUAL_POINT._coordinates[1] - ADJACENT_POINT._coordinates[1] >= e){
        return -1;
    }
    /// A hatarertek
    double theta_0 = M_PI / 4; // 45 fok

    Point p{};
    p._coordinates[0] = ADJACENT_POINT._coordinates[0] - ACTUAL_POINT._coordinates[0];
    p._coordinates[1] = ADJACENT_POINT._coordinates[1] - ACTUAL_POINT._coordinates[1];
    p._coordinates[2] = ADJACENT_POINT._coordinates[2] - ACTUAL_POINT._coordinates[2];
    double theta = atan(sqrt(pow(p._coordinates[0], 2) + pow(p._coordinates[2], 2)) / p._coordinates[1]);
    theta = fabs(theta);
    if(theta >= theta_0){
        return theta;
    }
    return 0;
}

/**
 * Kitorli a rossz, hibas pontokat (zajt)
 * (Egymast koveto pont paroknak meg kell egyeznie az x es z koordinatajuknak)
 * @param intersect_points a pontok listaja
 * @param e a hibahatar
 * @since 1.1
 */
void deleteWrongPoints(std::vector<Point> &intersect_points, double e) {
    for (int i = 0; i < (int) intersect_points.size() - 1; i++) {
        if (std::abs(intersect_points[i]._coordinates[0] - intersect_points[i + 1]._coordinates[0]) > e ||
            std::abs(intersect_points[i]._coordinates[2] - intersect_points[i + 1]._coordinates[2]) > e) {
            intersect_points.erase(intersect_points.begin() + i);
            i--;
        } else {
            i++;
        }
    }
}

/**
 * Megnezi, hogy a parameterkent kapott pont benne van-e a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param P a keresendo pont
 * @param l a hibahatarhoz az osztas egysege
 * @return benne van-e
 * @since 1.2
 */
bool isIncluded(std::vector<Point> &intersect_points, const Point &P, double l) {
    /// A bemeneti pontokat es a kimeneti pontokat kulon valogatja
    std::vector<double> input_points_y;
    std::vector<double> output_points_y;
    double e = l / 100; /// A hibahatar

    for(int i = 0; i < (int)intersect_points.size(); i++) {
        if (std::abs(intersect_points[i]._coordinates[0] - P._coordinates[0]) <= e &&
            std::abs(intersect_points[i]._coordinates[2] - P._coordinates[2]) <= e &&
            (std::abs(intersect_points[i]._coordinates[1] - P._coordinates[1]) <= e ||
             intersect_points[i]._coordinates[1] - P._coordinates[1] >= -e)) {
            if (i % 2 == 0) {
                input_points_y.push_back(intersect_points[i]._coordinates[1]);
            } else {
                output_points_y.push_back(intersect_points[i]._coordinates[1]);
            }
        }
    }

    if (!input_points_y.empty() && !output_points_y.empty()){
        /// Megkeresi a legkisebb y koordinataju bemeneti pontot, ami nagyobb mint a parameterkent kapott pont y koordinataja
        double min_y_input;
        std::sort(input_points_y.begin(), input_points_y.end());
        min_y_input = input_points_y[0];

        double min_y_output;
        std::sort(output_points_y.begin(), output_points_y.end());
        min_y_output = output_points_y[0];

        if ((min_y_output - min_y_input) >= -e){
            return true;
        }
    }
    return false;
}

/**
 * Kikeresi a parameterkent kapott pont a tombben (csal x-t es z-t vizsgalja)
 * @param intersect_points a pontok tombje
 * @param P a keresendo pont
 * @param l a hibahatarhoz az osztas egysege
 * @return visszaadja az y koordinatat a legkisebb, de az aktualisnal magasabb pontrol, ha nincs benne, akkor 0.0
 * @since 1.2
 */
double getY(std::vector<Point> &intersect_points, const Point &P, double l) {
    std::vector<double> input_points_y;
    std::vector<double> output_points_y;
    double e = l / 100; /// A hibahatar

    for(int i = 0; i < (int)intersect_points.size(); i++) {
        if ((std::abs(intersect_points[i]._coordinates[0] - P._coordinates[0]) <= e &&
             std::abs(intersect_points[i]._coordinates[2] - P._coordinates[2]) <= e) &&
            (std::abs(intersect_points[i]._coordinates[1] - P._coordinates[1]) <= e ||
             intersect_points[i]._coordinates[1] - P._coordinates[1] >= -e)) {

            if (i % 2 == 0) {
                input_points_y.push_back(intersect_points[i]._coordinates[1]);
            } else {
                output_points_y.push_back(intersect_points[i]._coordinates[1]);
            }
        }
    }

    if (!input_points_y.empty() && !output_points_y.empty()){
        /// Megkeresi a legkisebb y koordinataju bemeneti pontot, ami nagyobb mint a parameterkent kapott pont y koordinataja
        double min_y_input;
        std::sort(input_points_y.begin(), input_points_y.end());
        min_y_input = input_points_y[0];

        double min_y_output;
        std::sort(output_points_y.begin(), output_points_y.end());
        min_y_output = output_points_y[0];

        if ((min_y_output - min_y_input) >= -e){
            return min_y_input;
        }
    }
    return 0.0;
}

/**
 * Osszehasonlitja a ket kapott pont koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * Elorebb a kisebb y koordinataju pontok kerulnek
 * @param P_1 az eslo pont
 * @param P_2 a masodik pont
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareInputPointsYXZ(const Point &P_1, const Point &P_2) {
    if((P_1._coordinates[1] - P_2._coordinates[1]) < -P_1._e){
        return true;
    } else if(std::abs(P_1._coordinates[1] - P_2._coordinates[1]) <= P_1._e){
        if((P_1._coordinates[0] - P_2._coordinates[0]) < -P_1._e){
            return true;
        } else if(std::abs(P_1._coordinates[0] - P_2._coordinates[0]) <= P_1._e){
            if((P_1._coordinates[2] - P_2._coordinates[2]) < -P_1._e){
                return true;
            }
        }
    }
    return false;
}

/**
 * Osszehasonlitja a ket kapott el kezdopontjanak koordinatait es visszadja az elobbre levot
 * Elsonek az y koordinata alapjan, majd az x, majd a z koordinata alapjan
 * @param E_1 az eslo el
 * @param E_2 a masodik el
 * @return az elso elem elobbre valo-e vagy sem
 * @since 1.3
 */
bool compareEdgesInputPoints(const Edge& E_1, const Edge& E_2) {
    if((E_1.p1._coordinates[1] - E_2.p1._coordinates[1]) < -E_1.p1._e){
        return true;
    } else if(std::abs(E_1.p1._coordinates[1] - E_2.p1._coordinates[1]) <= -E_1.p1._e){
        if((E_1.p1._coordinates[0] - E_2.p1._coordinates[0]) < -E_1.p1._e){
            return true;
        } else if(std::abs(E_1.p1._coordinates[0] - E_2.p1._coordinates[0]) <= -E_1.p1._e){
            if((E_1.p1._coordinates[2] - E_2.p1._coordinates[2]) < -E_1.p1._e){
                return true;
            } else if(std::abs(E_1.p2._coordinates[1] - E_2.p2._coordinates[1]) <= -E_1.p1._e){
                if((E_1.p2._coordinates[0] - E_2.p2._coordinates[0]) < -E_1.p1._e){
                    return true;
                } else if(std::abs(E_1.p2._coordinates[0] - E_2.p2._coordinates[0]) <= -E_1.p1._e){
                    if((E_1.p2._coordinates[2] - E_2.p2._coordinates[2]) < -E_1.p1._e){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * A parameterkent kapott eleket sorbarendezi a rendezo fuggveny alapjan
 * @param edges az elek
 * @param COMPARE a rendezo fuggveny
 * @since 4.1
 */
void sort(std::vector<Edge> &edges, const std::function<bool(const Edge&, const Edge&)>& COMPARE) {
    for (int i = 0; i < (int)edges.size(); i++){
        for (int j = i + 1; j < (int)edges.size(); j++){
            if (COMPARE(edges[i], edges[j])){
                Edge temp = edges[i];
                edges[i] = edges[j];
                edges[j] = temp;
            }
        }
    }
}

/**
 * Kikeresi az elek kozul a pontot és visszaadja az indexet
 * @param points a pontok halmaza
 * @param p a keresett pont
 * @return
 */
int findPoint(std::vector<Point> &points, Point &p) {
    /// A szamokat kerekitjuk hat tizedes jegyre
    for (double & coordinate : p._coordinates) {
        coordinate = round( coordinate * 1000000) / 1000000;
    }

    for(int i = 0; i < (int)points.size(); i++){
        /// A szamokat kerekitjuk hat tizedes jegyre
        for (double & coordinate : points[i]._coordinates) {
            coordinate = round(coordinate * 1000000) / 1000000;
        }
        if(points[i] == p){
            return i;
        }
    }
    return -1;
}

/**
 * Beallitja a pontoknak, hogy milyen a sulyuk
 * @param edges az elek
 * @param input_points a pontok
 * @param max_weight a maximalis suly
 * @return a pontok listaja, aminke a sulyuk nem tul nagy
 * @since 1.3
 */
std::vector<Point>
setWeightAllPointsAndGetSupportPoints(std::vector<Edge> &edges, std::vector<Point> &input_points, double max_weight) {

    sort(edges, compareEdgesInputPoints);

    /// Kiszamolja minden pontra a sulyt
    for(auto & edge : edges){
        int p_1_index = findPoint(input_points, edge.p1);
        int p_2_index = findPoint(input_points, edge.p2);
        if(p_1_index == -1 || p_2_index == -1){
            continue;
        }

        double weight_p_2_actual = input_points[p_2_index]._weight;
        double weight_p_1_actual = input_points[p_1_index]._weight;
        double weight_p_2_new = input_points[p_1_index]._weight + edge.weight;
        double weight_edge = edge.weight;
        if(weight_p_2_actual == -1){
            if(weight_p_1_actual == -1){
                input_points[p_2_index]._weight = weight_edge;
                input_points[p_1_index]._weight = 0;
            }else{
                input_points[p_2_index]._weight = weight_p_2_new;
            }
        }else{
            if(weight_p_1_actual == -1) {
                input_points[p_2_index]._weight = weight_edge;
                input_points[p_1_index]._weight = 0;
            }else {
                if (weight_p_2_actual - weight_p_2_new > edge.p1._e) {
                    input_points[p_2_index]._weight = weight_p_2_new;
                }
            }
        }
    }

    /// Felveszi a csucspontokat a listaba
    std::vector<Point> support_points;
    for(auto & edge : edges){
        int p_1_index = findPoint(input_points, edge.p1);
        int p_2_index = findPoint(input_points, edge.p2);
        if(p_1_index == -1 || p_2_index == -1){
            continue;
        }
        support_points.push_back(input_points[p_1_index]);
        support_points.push_back(input_points[p_2_index]);
    }
    /// Kitorli a duplikatumokat
    std::sort(support_points.begin(), support_points.end(), compareInputPointsYXZ);
    for (int i = 0; i < (int)support_points.size() - 1; i++){
        if (support_points[i] == support_points[i + 1]){
            if (support_points[i]._weight == -1){
                support_points.erase(support_points.begin() + i);
                i--;
            } else {
                if (support_points[i + 1]._weight == -1){
                    support_points.erase(support_points.begin() + i + 1);
                    i--;
                } else {
                    if (support_points[i]._weight < support_points[i + 1]._weight){
                        support_points.erase(support_points.begin() + i + 1);
                        i--;
                    } else {
                        support_points.erase(support_points.begin() + i);
                        i--;
                    }
                }
            }
        }
    }

    /// Kitorli azokat a pontokat, amiknek mar tul nagy a sulya
    for (int j = 0; j < (int)support_points.size(); j++){
        if (support_points[j]._weight > max_weight){
            support_points.erase(support_points.begin() + j);
        }
    }

    return support_points;
}

/**
 * A parameterkent kapott eleket kiirja a .obj fileba
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @param INPUT_FILE_NAME a bemeneti file neve
 * @param edges a kiirando elek
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.2
 */
void writeInputEdges(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME, std::vector<Edge> &edges,
                     bool is_finish) {
    if (is_finish) {
        std::ofstream file(OUTPUT_FILE_NAME);
        if (!file) {
            std::cerr << "Error: The file " << OUTPUT_FILE_NAME << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Input edges generated from " << INPUT_FILE_NAME << " by BTMLYV\n";
        int k = 1;
        for (auto &edge: edges) {
            file << "v " << edge.p1._coordinates[0] << " " << edge.p1._coordinates[1] << " " << edge.p1._coordinates[2]
                 << "\n";
            file << "v " << edge.p2._coordinates[0] << " " << edge.p2._coordinates[1] << " " << edge.p2._coordinates[2]
                 << "\n";
            file << "l " << k << " " << k + 1 << "\n";
            k = k + 2;
        }
        file.close();
        writeLog("\tInput edges written to file");
    }
}

/**
 * A parameterkent kapott pontokat kiirja a .obj fileba
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @param INPUT_FILE_NAME az eredeti alakzat neve
 * @param count a szamlalo erteke
 * @param points a pontok
 * @param is_finish igaz, ha a vegso kiiratasrol van szo
 * @since 1.3
 */
void
writePoints(const std::string &OUTPUT_FILE_NAME, const std::string &INPUT_FILE_NAME, int count, std::vector<Point> &points,
            bool is_finish) {
    if (is_finish) {
        std::ofstream file(OUTPUT_FILE_NAME);
        if (!file) {
            std::cerr << "Error: The file " << OUTPUT_FILE_NAME << " cannot be opened!" << std::endl;
            exit(1);
        }
        /// A kimeneti file fejlece
        file << "# Supported points No. " << count << " generated from " << INPUT_FILE_NAME << " by BTMLYV\n";
        for (auto &point: points) {
            file << "v " << point._coordinates[0] << " " << point._coordinates[1] << " " << point._coordinates[2] << "\n";
        }
        file.close();
    }
}

/**
 * Kiirja a futtatas adatait a konzolra
 * @param INPUT_FILE_NAME
 */
void writeStartLog(const std::string &INPUT_FILE_NAME) {
    std::cout << "   _____                              _      _____                           _   _             \n"
                 "  / ____|                            | |    / ____|                         | | (_)            \n"
                 " | (___  _   _ _ __  _ __   ___  _ __| |_  | |  __  ___ _ __   ___ _ __ __ _| |_ _  ___  _ __  \n"
                 "  \\___ \\| | | | '_ \\| '_ \\ / _ \\| '__| __| | | |_ |/ _ \\ '_ \\ / _ \\ '__/ _` | __| |/ _ \\| '_ \\ \n"
                 "  ____) | |_| | |_) | |_) | (_) | |  | |_  | |__| |  __/ | | |  __/ | | (_| | |_| | (_) | | | |\n"
                 " |_____/ \\__,_| .__/| .__/ \\___/|_|   \\__|  \\_____|\\___|_| |_|\\___|_|  \\__,_|\\__|_|\\___/|_| |_|\n"
                 "              | |   | |                                                                        \n"
                 "              |_|   |_|                                                                        " << std::endl;
    std::cout << " for 3D printing" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "\nThe input file is: " << INPUT_FILE_NAME << std::endl;
    std::cout << "Log:-------------------------------------------------------------------------------------------" << std::endl;
}

/**
 * Kiirja hogy keszen van
 */
void writeEndLog() {
    std::cout << "-----------------------------------------------------------------------------------------------\n" << std::endl;
    std::cout << "\x1B[32m   _____                      _      _           _ \n"
                 "  / ____|                    | |    | |         | |\n"
                 " | |     ___  _ __ ___  _ __ | | ___| |_ ___  __| |\n"
                 " | |    / _ \\| '_ ` _ \\| '_ \\| |/ _ \\ __/ _ \\/ _` |\n"
                 " | |___| (_) | | | | | | |_) | |  __/ ||  __/ (_| |\n"
                 "  \\_____\\___/|_| |_| |_| .__/|_|\\___|\\__\\___|\\__,_|\n"
                 "                       | |                         \n"
                 "                       |_|                         \033[0m" << std::endl;
}

/**
 * Kiirja a logot a konsolra
 * @param log a kiirando szoveg
 */
void writeLog(const std::string &log) {
    /// Az aktuális idő lekérdezése
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_now = std::localtime(&now_c);

    std::cout << std::put_time(local_now, "%Y/%m/%d %H:%M:%S") << log << std::endl;
}

/**
 * Keresztszorzatot szamol ket pont kozott
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a keresztszorzat
 * @since 3.1
 */
Point crossProduct(Point p_1, Point p_2){
    Point p;
    p._coordinates[0] = p_1._coordinates[1] * p_2._coordinates[2] - p_1._coordinates[2] * p_2._coordinates[1];
    p._coordinates[1] = p_1._coordinates[2] * p_2._coordinates[0] - p_1._coordinates[0] * p_2._coordinates[2];
    p._coordinates[2] = p_1._coordinates[0] * p_2._coordinates[1] - p_1._coordinates[1] * p_2._coordinates[0];
    return p;
}

/**
 * Skalaris szorzatot szamol ket pont kozott
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a skalaris szorzat
 * @since 3.1
 */
double dotProduct(Point &p_1, Point &p_2){
    return p_1._coordinates[0] * p_2._coordinates[0] +
           p_1._coordinates[1] * p_2._coordinates[1] +
           p_1._coordinates[2] * p_2._coordinates[2];
}

/**
 * Megkeresi a minimum es a maximum ertekeket
 * @param mesh_object az alakzat
 * @param COORDINATE melyik tengely menten keressen
 * @return a minimum es a maximum ertekek egy vektorban
 * @since 4.1
 */
std::vector<double> findMinMax(MyMesh &mesh_object, const std::string& COORDINATE){
    std::vector<double> vec;
    vec.push_back(1000000);
    vec.push_back(-1000000);

    int k;
    if (COORDINATE == "x" ){
        k = 0;
    } else if (COORDINATE == "z"){
        k = 2;
    } else if(COORDINATE == "y"){
        k = 1;
    } else {
        std::cerr << "Error: Wrong coordinate!" << std::endl;
        exit(1);
    }

    for(auto v_it = mesh_object.vertices_begin(); v_it != mesh_object.vertices_end(); ++v_it){
        if(mesh_object.point(*v_it)[k] < vec[0]){
            vec[0] = mesh_object.point(*v_it)[k];
        }
        if(mesh_object.point(*v_it)[k] > vec[1]){
            vec[1] = mesh_object.point(*v_it)[k];
        }
    }
    return vec;
}

/**
 * Kiszamolja az alatamasztas ellenorzesi tavolsagat
 * @param l az alatamasztas ellenorzesi tavolsaga
 * @param diameter atamasz atmeroje
 * @param mesh_object az alakzat
 * @since 4.1
 */
void calculateDiameterAndL(double &l, double &diameter, MyMesh &mesh_object){
    std::vector<double> X = findMinMax(mesh_object, "x");
    std::vector<double> Y = findMinMax(mesh_object, "y");
    std::vector<double> Z = findMinMax(mesh_object, "z");

    double deltaX = X[1] - X[0];
    double deltaY = Y[1] - Y[0];
    double deltaZ = Z[1] - Z[0];
    double max = std::max(deltaX, std::max(deltaY, deltaZ));
    l = max / 20.3; /// Azert nem kerek szam, hatha igy kevesebb lesz a double ertekbol fakado hiba
    diameter = 2 * l;
}

/**
 * A ket kapott pont kozotti tavolsagot adja vissza
 * @param p_1 az elso pont
 * @param p_2 a masodik pont
 * @return a ket pont kozotti tavolsag
 * @since 3.1
 */
double getDistance(Point &p_1, Point &p_2){
    return sqrt(pow(p_1._coordinates[0] - p_2._coordinates[0], 2) +
                pow(p_1._coordinates[1] - p_2._coordinates[1], 2) +
                pow(p_1._coordinates[2] - p_2._coordinates[2], 2));
}

/**
 * Kiszamolja az elek ossztavolsagat
 * @param support_lines az alatamasztasi elek
 * @return az ossz tavolsag
 * @since 4.1
 */
double calculatePoint(std::vector<Point>& support_lines){
    double point = 0;
    for (int i = 0; i < (int)support_lines.size(); i = i + 2){
        point += getDistance(support_lines[i], support_lines[i + 1]);
    }
    return point;
}

/**
 * Kiszamolja az elek ossztavolsagat
 * @param support_lines az alatamasztasi elek
 * @return az ossz tavolsag
 * @since 4.1
 */
double calculatePoint(std::vector<Edge>& support_lines){
    double point = 0;
    for (auto & supportLine : support_lines){
        point += getDistance(supportLine.p1, supportLine.p2);
    }
    return point;
}

/**
 * Kiirja a mesht a megadott fileba
 * @param mesh_object az alakzat
 * @param OUTPUT_FILE_NAME a kimeneti file neve
 * @since 4.1
 */
void writeMesh(MyMesh &mesh_object, const std::string &OUTPUT_FILE_NAME) {
    if(!OpenMesh::IO::write_mesh(mesh_object, OUTPUT_FILE_NAME)){
        std::cerr << "Error: Cannot write mesh to " << OUTPUT_FILE_NAME << std::endl;
        exit(1);
    }
}