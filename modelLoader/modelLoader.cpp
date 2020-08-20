//model loader program Dom Ralphs November 2019
#include <iostream>
#include <GL/glut.h>
#include <glm/glm.hpp> //includes GLM
#include <glm/gtx/component_wise.hpp>
#include <vector>
#include <fstream>
#include <sstream>


using namespace std;
using namespace glm;

//struct for storing the data from the file
struct Vertex
{
	vec3 position; //postion vector of 3 which is x,y z postion of vertices
	vec2 texcoord; //textcord vector of 2 which is x,y  postion of textcords
	vec3 normal; //normals vector of 3 whihc is the x,y and z postion of normals
};

//struct for storing the temp data from the file
struct VertRef
{
	VertRef(int v, int vt, int vn) //vert ref for v = vertices, vt =  vertex textcords, vn = vertex normals
		: v(v), vt(vt), vn(vn) // used to store the data in each int.
	{ }
	int v, vt, vn;
};


vector< Vertex > LoadOBJ(istream& in) //class of vector, LoadOBJ struct for storing the data from the file
{
	vector< Vertex > verts; //set up a vector for the verts

	vector< vec4 > positions(1, vec4(0, 0, 0, 0)); //set up the positions as a vec of 4
	vector< vec3 > texcoords(1, vec3(0, 0, 0)); // set up textcords as a vector of 3
	vector< vec3 > normals(1, vec3(0, 0, 0)); // set the normals as a vector of 3


	//read the obj file
	string line; //set line as a string
	while (getline(in, line))  //while there is data, read each line
	{
		istringstream lineInfo(line); // set up the lineInfo as a stringstream using the line data
		string lineType; //set up the linetype as a string, used for checking the first letter of the line
		lineInfo >> lineType; //lineInfo into line type


		//check for vertex  
		if (lineType == "v") //check if the start of the line begains with a V
		{
			float x = 0, y = 0, z = 0, w = 1;   //set up the floats to store each data cord in
			lineInfo >> x >> y >> z >> w; // add each cord data to the floats x = vertex data x and  y  = vertex data y etc...
			positions.push_back(vec4(x, y, z, w)); //add the data of x,y,z and w into the positions vector
		}

		//check for texture cords
		if (lineType == "vt") //check if the start of the line begains with a vt
		{
			float u = 0, v = 0, w = 0; //set up the floats to store each text data cord in
			lineInfo >> u >> v >> w; // add each texture cord data to the floats u = texture data x and  v  = vertex data y etc...
			texcoords.push_back(vec3(u, v, w)); //add the data of u, v and w into the textcords vector
		}

		//check for normals cords
		if (lineType == "vn") //check if the start of the line begains with a vn
		{
			float i = 0, j = 0, k = 0; //set up the floats to store each normals data cord in
			lineInfo >> i >> j >> k;  // add each normal cord data to the floats i = normal data x and j = normal data y etc....
			normals.push_back(normalize(vec3(i, j, k))); //add the data of i, j and k into the normals vector
		}

		//check for faces
		if (lineType == "f") //check if the start of the line begains with a f
		{
			vector< VertRef > refs; //set up a struct to store the refrences for the faces
			string refStr;  //set up a string called ref string
			while (lineInfo >> refStr) //while there is data to read
			{
				istringstream ref(refStr); // set string object as a string stream
				string vStr, vtStr, vnStr;  //set up strings for vertex String vStr, texture string vtStr, normal string vnStr to split the faces data up

				getline(ref, vStr, '/');  // split the faces up with / vertex string as vStr
				getline(ref, vtStr, '/'); // split the faces up with / texture string as vtSTr
				getline(ref, vnStr, '/'); // split the faces up with / normal string as vnStr

				int v = atoi(vStr.c_str());  //copy vStr into a c-string and use atoi to convert to int
				int vt = atoi(vtStr.c_str()); //copy vtStr into a c-string and use atoi to convert to int
				int vn = atoi(vnStr.c_str()); //copy vnStr into a c-string and use atoi to convert to int

				v = (v >= 0 ? v : positions.size() + v); //ternary operator, if v >= 0 then v // else  size of vertices + v
				vt = (vt >= 0 ? vt : texcoords.size() + vt); //ternary operator, if vt >= 0 then vt // else  size of vertices + vt
				vn = (vn >= 0 ? vn : normals.size() + vn); //ternary operator, if vn >= 0 then vn // else  size of vertices + vn
				refs.push_back(VertRef(v, vt, vn)); //add element v,vt and vn to end of vector verTexRef
			}

			if (refs.size() < 3) //check if face ref is less than 3 
			{
				//if its less than 3 it isn't in the correct format or isn't a face ref so skip
				printf("err faces can't be read, not correct format \n");
				continue;
			}

			// triangulate, assuming n>3 poloygons are convex (all interier angles are less than 180 degrees) and coplanar ()
			VertRef* p[3] = { &refs[0], NULL, NULL }; //store data in p 
			for (size_t i = 1; i + 1 < refs.size(); ++i) //size_t unasigned int, for loop incrementimg before the expression is evaluated 
			{
				p[1] = &refs[i + 0]; //point to the memory location of the first FaceRefs
				p[2] = &refs[i + 1]; //point to the memory location of the second FaceRefs

				vec3 U(positions[p[1]->v] - positions[p[0]->v]); //add data to the vector
				vec3 V(positions[p[2]->v] - positions[p[0]->v]); //add data to the vector
				vec3 faceNormal = normalize(cross(U, V)); //check for crossed correlation between the vectors of U and V

				for (size_t j = 0; j < 3; ++j) //for inner loop to add the data
				{
					Vertex vert; //set vert in the vertex
					vert.position = vec3(positions[p[j]->v]); //vert position = vector of 3
					vert.texcoord = vec2(texcoords[p[j]->vt]); //vert textcords = vector of 2
					vert.normal = (p[j]->vn != 0 ? normals[p[j]->vn] : faceNormal); //if p is less or greater than  not 0 then normals p is less or greater then vn ie.. the plain is 0 else face normal
					verts.push_back(vert); // add data from vert back to the verts.
				}
			}
		}
	}

	return verts; //return the verts
}


//model 1
vector< Vertex > model; //set the vector for the object model
void display()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //set colour of background 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //set buffer
	glMatrixMode(GL_PROJECTION); //set the matrix projection
	glLoadIdentity();
	double w = glutGet(GLUT_WINDOW_WIDTH); //set center point
	double h = glutGet(GLUT_WINDOW_HEIGHT); //set center point
	double ar = w / h;
	gluPerspective(60, ar, 0.1, 100); //perspective camera view
	glMatrixMode(GL_MODELVIEW); //set matrix model view
	glLoadIdentity(); //load identy matrix
	glTranslatef(0, 0, -10); //set translate of matrix
	glPushMatrix(); //add matrix to pipeline
	{
		// setup object
		glColor3ub(19, 122, 154); //set object colour
		glEnableClientState(GL_VERTEX_ARRAY); //get vertex array
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); //get texture cord array
		glEnableClientState(GL_NORMAL_ARRAY); //get normals array
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &model[0].position); //refrence vertex postion pointer
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &model[0].texcoord); //reference ctextcoord pointer 
		glNormalPointer(GL_FLOAT, sizeof(Vertex), &model[0].normal); //refreence normal pointer
		glDrawArrays(GL_TRIANGLES, 0, model.size()); // draw the arrays
		glDisableClientState(GL_VERTEX_ARRAY); //discard the client states to use the buffers
		glDisableClientState(GL_TEXTURE_COORD_ARRAY); //discard the client states to use the buffers
		glDisableClientState(GL_NORMAL_ARRAY); //discard the client states to use the buffers
	}
	glPopMatrix(); //remove matrix from  stack
	glutSwapBuffers(); //swap the buffers
}

// return the min/max points of pts
template< typename Vec >
pair< Vec, Vec > GetExtents(const Vec* pts, size_t stride, size_t count) //findong the min and max points fro the transformations
{
	unsigned char* base = (unsigned char*)pts;
	Vec pmin(*(Vec*)base);
	Vec pmax(*(Vec*)base);
	for (size_t i = 0; i < count; ++i, base += stride)
	{
		const Vec& pt = *(Vec*)base;
		pmin = glm::min(pmin, pt); //set the min points
		pmax = glm::max(pmax, pt); //set the max points
	}

	return make_pair(pmin, pmax);
}

// Centers geometry around the origin of the object and then scales to fit in the window 
template< typename Vec >
void CenterAndScale(Vec* pts, size_t stride, size_t count, const typename Vec::value_type& size) //set up center and scale function 
{
	typedef typename Vec::value_type Scalar;

	// get min and the max extents
	pair< Vec, Vec > exts = GetExtents(pts, stride, count);

	// center and scale for the object  									
	const Vec center = (exts.first * Scalar(0.5)) + (exts.second * Scalar(0.5f));

	const Scalar factor = size / glm::compMax(exts.second - exts.first);
	unsigned char* base = (unsigned char*)pts;
	for (size_t i = 0; i < count; ++i, base += stride)
	{
		Vec& pt = *(Vec*)base;
		pt = ((pt - center) * factor);
	}
}


//set main program
int main(int argc, char** argv)
{
	//file locations for easy changing when testing 
	//ifstream myFile("media/textures/Creeper-obj/Creeper.obj"); //Creeper obj
	//ifstream myFile("media/textures/pouf-obj/Pouf.obj"); //pouf obj
	ifstream myFile("media/textures/LowPolyBoat-obj/low_poly_boat.obj"); //Boat obj
	model = LoadOBJ(myFile);
	CenterAndScale(&model[0].position, sizeof(Vertex), model.size(), 5); // scale and size of the model 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE); //set diasplay mode
	glutInitWindowSize(800, 800); //set window size 
	glutCreateWindow("Model Loader"); //set window name 
	glutDisplayFunc(display); //set display 
	glEnable(GL_DEPTH_TEST); //enable and use the depth test to store z cords and check depth so triangle render in the correct places
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //fill object with colour
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	glutMainLoop(); //start main loop
	return 0;
} //end of main