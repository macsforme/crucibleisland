// Mesh.h
// Crucible Island

#ifndef MESH_H
#define MESH_H

// program headers
#include "math/VectorMath.h"
#include "platform/Platform.h"

// library headers
#include <fstream>
#include <map>
#include <string>
#include <vector>

// global variables
extern Platform* platform;

// class definition
class Mesh {
public:
	struct Face {
		unsigned int vertices[3];
		unsigned int normals[3];
		unsigned int texCoords[3];
	};

	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;

	std::map< std::string,std::vector<Face> > faceGroups;

	// vertex manipulation
	void addVertex(Vector3 vertex) { vertices.push_back(vertex); }
	void changeVertex(unsigned int index, Vector3 vertex) {
		vertices[index].x = vertex.x;
		vertices[index].y = vertex.y;
		vertices[index].z = vertex.z;
	}
	void changeVertex(unsigned int index, float x, float y, float z) {
		vertices[index].x = x;
		vertices[index].y = y;
		vertices[index].z = z;
	}

	// normal manipulation
	void addNormal(Vector3 normal) { normals.push_back(normal); }
	void changeNormal(unsigned int index, Vector3 normal) {
		normals[index].x = normal.x;
		normals[index].y = normal.y;
		normals[index].z = normal.z;
	}
	void changeNormal(unsigned int index, float x, float y, float z) {
		normals[index].x = x;
		normals[index].y = y;
		normals[index].z = z;
	}

	// texture coordinate manipulation
	void addTexCoord(Vector2 texCoord) { texCoords.push_back(texCoord); }
	void changeTexCoord(unsigned int index, Vector2 texCoord) {
		texCoords[index].x = texCoord.x;
		texCoords[index].y = texCoord.y;
	}
	void changeTexCoord(unsigned int index, float x, float y) {
		texCoords[index].x = x;
		texCoords[index].y = y;
	}

	// face manipulation
	void addFace(
			unsigned int vert1Index,
			unsigned int vert2Index,
			unsigned int vert3Index,
			std::string group = ""
		) {
		Face face;

		face.vertices[0] = vert1Index;
		face.vertices[1] = vert2Index;
		face.vertices[2] = vert3Index;

		faceGroups[group].push_back(face);
	}
	void addFace(
			unsigned int vert1Index,
			unsigned int vert2Index,
			unsigned int vert3Index,
			unsigned int norm1Index,
			unsigned int norm2Index,
			unsigned int norm3Index,
			std::string group = ""
		) {
		Face face;

		face.vertices[0] = vert1Index;
		face.vertices[1] = vert2Index;
		face.vertices[2] = vert3Index;

		face.normals[0] = norm1Index;
		face.normals[1] = norm2Index;
		face.normals[2] = norm3Index;

		faceGroups[group].push_back(face);
	}
	void addFace(
			unsigned int vert1Index,
			unsigned int vert2Index,
			unsigned int vert3Index,
			unsigned int norm1Index,
			unsigned int norm2Index,
			unsigned int norm3Index,
			unsigned int texCoord1Index,
			unsigned int texCoord2Index,
			unsigned int texCoord3Index,
			std::string group = ""
		) {
		Face face;

		face.vertices[0] = vert1Index;
		face.vertices[1] = vert2Index;
		face.vertices[2] = vert3Index;

		face.normals[0] = norm1Index;
		face.normals[1] = norm2Index;
		face.normals[2] = norm3Index;

		face.texCoords[0] = texCoord1Index;
		face.texCoords[1] = texCoord2Index;
		face.texCoords[2] = texCoord3Index;

		faceGroups[group].push_back(face);
	}

	// utility methods
	void autoNormal() {
		normals.clear();

		std::map<unsigned int, std::vector<Vector3> > vertexNormals;

		for(
				std::map< std::string,std::vector<Face> >::iterator groupItr =
						faceGroups.begin();
				groupItr != faceGroups.end();
				++groupItr
			) {
			for(
					size_t index = 0;
					index < groupItr->second.size();
					++index
				) {
				Vector3 v1(
						vertices[groupItr->second[index].vertices[1]] -
						vertices[groupItr->second[index].vertices[0]]
					);
				Vector3 v2(
						vertices[groupItr->second[index].vertices[2]] -
						vertices[groupItr->second[index].vertices[1]]
					);

				Vector3 normal = cross(v1, v2);
				normal.norm();

				vertexNormals[groupItr->second[index].vertices[0]].push_back(normal);
				vertexNormals[groupItr->second[index].vertices[1]].push_back(normal);
				vertexNormals[groupItr->second[index].vertices[2]].push_back(normal);

				groupItr->second[index].normals[0] = groupItr->second[index].vertices[0];
				groupItr->second[index].normals[1] = groupItr->second[index].vertices[1];
				groupItr->second[index].normals[2] = groupItr->second[index].vertices[2];
			}
		}

		for(size_t i = 0; i < vertices.size(); ++i) {
			unsigned int count = 0;
			Vector3 total(0.0f, 0.0f, 0.0f);

			for(size_t p = 0; p < vertexNormals[i].size(); ++p) {
				++count;
				total += vertexNormals[i][p];
			}

			total /= count;
			normals.push_back(total);
		}
	}
	void autoTexCoord(unsigned int index, std::string group = "") {
		texCoords.push_back(Vector2(
				vertices[faceGroups[group][index].vertices[0]].x,
				vertices[faceGroups[group][index].vertices[0]].y
			));
		faceGroups[group][index].texCoords[0] = texCoords.size() - 1;

		texCoords.push_back(Vector2(
				vertices[faceGroups[group][index].vertices[1]].x,
				vertices[faceGroups[group][index].vertices[1]].y
			));
		faceGroups[group][index].texCoords[1] = texCoords.size() - 1;

		texCoords.push_back(Vector2(
				vertices[faceGroups[group][index].vertices[2]].x,
				vertices[faceGroups[group][index].vertices[2]].y
			));
		faceGroups[group][index].texCoords[2] = texCoords.size() - 1;
	}

	// constructors
	Mesh() { };
	Mesh(std::string filename) {
		std::ifstream objFile;
		std::string line;

		std::string groupName = "default";

		objFile.open(std::string(platform->dataPath + "/data/models/" + filename + ".obj").c_str());
		if(! objFile.is_open())
			gameSystem->log(GameSystem::LOG_FATAL, "Unable to load model " + filename + " from file.");

		while(! objFile.eof()) {
			std::getline(objFile, line);

			if(line.length() > 0) {
				// parse the line
				std::string lineType = line.substr(0,line.find(" "));

				// for the valid lines we use, store the data
				if(lineType == "g") {
					// group definition
					groupName = line.substr(2, line.length() - 2);
				} else if(lineType == "s") {
					// smooth face... we ignore this
				} else if(lineType == "v") {
					// vertex definition
					float subjs[3] = { };
					sscanf(line.c_str() + 2 * sizeof(char), "%f %f %f", &subjs[0], &subjs[1], &subjs[2]);

					addVertex(Vector3(subjs[0], subjs[1], subjs[2]));
				} else if(lineType == "vt") {
					// texture coordinate
					float subjs[2] = { };
					sscanf(line.c_str() + 2 * sizeof(char), "%f %f", &subjs[0], &subjs[1]);

					addTexCoord(Vector2(subjs[0], subjs[1]));
				} else if(lineType == "vn") {
					// normals
					float subjs[3] = { };
					sscanf(line.c_str() + 2 * sizeof(char), "%f %f %f", &subjs[0], &subjs[1], &subjs[2]);

					addNormal(Vector3(subjs[0], subjs[1], subjs[2]));
				} else if(lineType == "f") {
					// face definition (supports triangles and quads only)
					unsigned int faceVertices[4];
					unsigned int faceNormals[4];
					unsigned int faceTexCoords[4];

					char subjs[4][20] = { };
					int numFaces = sscanf(line.c_str() + 2 * sizeof(char), "%s %s %s %s", subjs[0], subjs[1], subjs[2], subjs[3]);

					for(int i = 0; i < numFaces; ++i) {
						// process each vertex/textcoord/normal
						unsigned int vertex = 0;
						unsigned int texCoord = 0;
						unsigned int normal = 0;

						if(sscanf(subjs[i], "%u/%u/%u", &vertex, &texCoord, &normal) == 3) {
							// wavefront indices start at 1 but our indices start at 0
							vertex -= 1;
							texCoord -= 1;
							normal -= 1;

							// line has slashes and specifies all three
							if(vertex >= vertices.size() || normal >= normals.size() || texCoord >= texCoords.size())
								gameSystem->log(GameSystem::LOG_FATAL, "Model face specification has out-of-bounds index.");

							faceVertices[i] = vertex;
							faceNormals[i] = normal;
							faceTexCoords[i] = texCoord;
						} else if(sscanf(subjs[i], "%u//%u", &vertex, &normal) == 2) {
							// line has slashes and specifies the vertex and normal
							if(vertex > vertices.size() || normal > normals.size())
								gameSystem->log(GameSystem::LOG_FATAL, "Model face specification has out-of-bounds index.");

							faceVertices[i] = vertex;
							faceNormals[i] = normal;
						} else if(sscanf(subjs[i],"%u", &vertex)) {
							// line has just the vertex
							if(vertex > vertices.size())
								gameSystem->log(GameSystem::LOG_FATAL, "Model face specification has out-of-bounds index.");
							faceVertices[i] = vertex;
						} else {
							gameSystem->log(
									GameSystem::LOG_FATAL,
									"Model face specification has an invalid format."
								);
						}
					}

					// save it
					addFace(
							faceVertices[0],
							faceVertices[1],
							faceVertices[2],
							faceNormals[0],
							faceNormals[1],
							faceNormals[2],
							faceTexCoords[0],
							faceTexCoords[1],
							faceTexCoords[2],
							groupName
						);
					// add the other half of the quad as a second triangle, if necessary
					if(numFaces == 4)
						addFace(
								faceVertices[2],
								faceVertices[3],
								faceVertices[0],
								faceNormals[2],
								faceNormals[3],
								faceNormals[0],
								faceTexCoords[2],
								faceTexCoords[3],
								faceTexCoords[0],
								groupName
							);
				}
			}
		}

		objFile.close();
	}
};

#endif // MESH_H
