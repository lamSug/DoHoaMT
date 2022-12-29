//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;
GLfloat l = -0.75, r = 0.75;
GLfloat bottom = -0.25, top = 0.5;
GLfloat zNear = 1, zFar = 5;
GLfloat eye_x = 2.8, eye_y = 1.5, eye_z = 1.4;


/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(0.0, 0.0, 1.0, 0.0);//vị trí chiếu sáng

//các hệ số cường độ ánh sáng
color4 light_ambient(0.1, 0.1, 0.1, 1.0);//ánh sáng môi trường
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);//ánh sáng khuếch tán
color4 light_specular(1.0, 1.0, 1.0, 1.0);//ánh sáng phản chiếu sáng chói

//các hệ số hấp thụ ánh sáng(các thuộc tính của chất liệu)
color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;//màu phản xạ ra để mắt người nhìn thấy
color4 specular_product = light_specular * material_specular;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints+16]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints+16]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints+16];

point4 vertices[13]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[13]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint model_view_loc;
GLuint projection_loc;
GLuint view_loc;
mat4 instance;
mat4 instance_2;
mat4 instance_3;
mat4 model_view;
mat4 model_quay;
GLfloat theta[] = { 0 , 0, 0 };
GLfloat n;
GLfloat rotateNha[] = { 0,0,0 };
GLfloat rotateMain[] = { 0,0,0 };
GLfloat  rotateCanhTraiCuaSo, rotateCanhPhaiCuaSo, rotateCuaRaVao = 0;
mat4 transformNha , transformMain;
GLfloat transX[] = { 0, 0, 0 }, transY[] = { 0 }, transZ[] = { 0 };

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	 
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}

void initPyra() 
{
	vertices[8] = point4(-0.5, 0, -0.5, 1.0);
	vertices[9] = point4(0.5, 0, 0.5, 1.0);
	vertices[10] = point4(-0.5, 0, 0.5, 1.0);
	vertices[11] = point4(0.5, 0, -0.5, 1.0);
	vertices[12] = point4(0, 0.5, 0, 1.0);

	vertex_colors[8] = color4(1.0, 1.0, 0.0, 1.0);
	vertex_colors[9] = color4(1.0, 1.0, 0.0, 1.0); 
	vertex_colors[10] = color4(1.0, 1.0, 0.0, 1.0); 
	vertex_colors[11] = color4(1.0, 1.0, 0.0, 1.0);
	vertex_colors[12] = color4(1.0, 1.0, 0.0, 1.0);
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/

{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void quad2(int a, int b, int c)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/

{

colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void makeColorPyra(void) {
	quad(12, 8, 9, 10);
	quad(12, 10, 11, 8);
	//quad(12, 8, 9, 11);
	//
	//quad2(12, 8, 9);
	//quad2(12, 10, 9);
	//quad2(12, 11, 8);
	//quad2(12, 11, 10);
	//quad(8, 9, 10, 11);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
	initPyra();
	makeColorPyra();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_view_loc = glGetUniformLocation(program, "Model_View");
	view_loc = glGetUniformLocation(program, "view");
	projection_loc = glGetUniformLocation(program, "Projection");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glClearColor(1.0, 1.0, 1.0, 1.0);         /* Thiết lập màu trắng là màu xóa màn hình*/
}
void matSan(GLfloat rongMatSan, GLfloat caoMatSan, GLfloat dayMatSan)
{
	instance = Scale(rongMatSan, caoMatSan, dayMatSan);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE,instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tuongbao(GLfloat rongTuongBao, GLfloat caoTuongBao, GLfloat dayTuongBao)
{
	instance = Translate(0, 0, 0) * Scale(rongTuongBao , caoTuongBao ,dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.2f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.4f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.6f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.8f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.2f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.4f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.6f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 *instance );
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.8f) * Scale(rongTuongBao, caoTuongBao, dayTuongBao);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


}

void khungCua(GLfloat rongkhungCua, GLfloat caoKhungCua, GLfloat dayKhungCua)
{
	//Khung cua ben trai
	instance = Translate(0, 0, 1.2f) * Scale( rongkhungCua, caoKhungCua,  dayKhungCua);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//khung cua ben phai
	instance = Translate(0, 0, 0.45f) * Scale(rongkhungCua, caoKhungCua, dayKhungCua);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


}
void tuongTruoc(GLfloat rongTuongTruoc, GLfloat caoTuongTruoc, GLfloat dayTuongTruoc)
{
	instance = Translate(0, 0, 0) * Scale( rongTuongTruoc,  caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate(0, 0, 0.2f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.4f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	/*instance = Translate(0, 0, 1.2f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);*/

	instance = Translate(0, 0, 1.4f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.6f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 1.8f) * Scale(rongTuongTruoc, caoTuongTruoc, dayTuongTruoc);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void khungBaoVe(GLfloat rongkhung, GLfloat caoKhung, GLfloat dayKhung)
{
	instance = Translate(0, 0, 1.62f) * Scale( rongkhung,  caoKhung,  dayKhung);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, 0.2f) * Scale(rongkhung, caoKhung, dayKhung);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance_2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void veSan()
{
	material_diffuse = vec4(1, 1, 0.7, 1);   // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	instance = identity();
	matSan(5.0f, 0.03f, 4);

	material_diffuse = vec4(1, 1, 0.5, 1);   // mau vat
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);


	//Mặt tương sau
	instance_2 = Translate(-0.5 * 4 + 0.1, 0.5 *0.4f, -0.5 * 2 + 0.1);
	tuongbao(0.02f, 0.4f, 1.25f);

	//Mặt tương phải

	instance_2 = Translate(-0.5 * 2 + 0.1, 0.5 * 0.4f, -0.5*3) * RotateY(90);
	tuongbao(0.02f, 0.4f, 2.0f);

	//Mặt tường trái
	instance_2 = Translate(-0.5 * 2 + 0.1, 0.5 * 0.4f, 0.5 * 3) * RotateY(90);
	tuongbao(0.02f, 0.4f, 2.0f);

}
GLfloat x = 0.2, y = 0.2, z = 0.2;
GLfloat xx = 0, yy = 0, zz = 0, a = 0, b = 0, c = 0, z_nk = 0, m, i;
GLfloat xxe = -0.5, yxe = 0, zxe = 1.0, axe = 0, bxe = 0, cxe = 0;
GLfloat xoay[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

mat4 model, model_view1, instance_temp1;
void thanxe()
{
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	//instance = identity();
	instance = Translate(x, 0.35 * y + 0.5 * (0.1 * y), 0.0) * Scale(x, 0.7 * y, z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0.0, 0.0, 0.0) * Scale(3 * x, 0.1 * y, z) * RotateY(180);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(x, 0.4 * y + 0.5 * (0.1 * y), 0.5 * z) * Scale(0.35 * x, 0.4 * y, 0.3 * z) * RotateY(180);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(x, 0.4 * y + 0.5 * (0.1 * y), -0.5 * z) * Scale(0.35 * x, 0.4 * y, 0.3 * z) * RotateY(180);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thungxe()
{
	/*
	instance = identity();*/
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	instance = Translate(x - 0.1 * x, 0.055 * y, 0.0) * Scale(1.75 * x, 0.01 * y, z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(x - 0.115 * x, 0.3 * y + 0.06 * y, -0.5 * z + 0.005 * z) * Scale(1.75 * x, 0.6 * y, 0.01 * z) * RotateY(180);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(x - 0.115 * x, 0.3 * y + 0.06 * y, 0.5 * z - 0.005 * z) * Scale(1.75 * x, 0.6 * y, 0.01 * z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(2 * x - 0.25 * x, 0.3 * y + 0.06 * y, 0.0) * Scale(0.01 * x, 0.6 * y, z - 0.01 * y);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void ngan()
{
	//instance = identity();
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = Translate(0.0, -0.3 * y, 0.0) * Scale(0.01 * x, 0.6 * y, z - 0.01 * y);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void Banhxe()
{
	material_diffuse = vec4(0, 0.2, 0.2, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance = identity();
	instance = Translate(0.0, 0.0, 0.0) * Scale(0.1 * x, 0.35 * y, 0.2 * z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void obanhxe()
{
	instance = identity();
	instance = Translate(0.0, 0.0, 0.0) * Scale(0.01 * x, 0.01 * y, 0.01 * z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model * model_view1 * instance_temp1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void xe()
{
	instance_temp1 = Translate(0.1 + xxe,0.02+ yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	thanxe();
	instance_temp1 *= Translate(-1.5 * x, 0, 0.0) * RotateZ(m);
	thungxe();
	instance_temp1 *= Translate(0.0, 0.6 * y + 0.06 * y, 0.0) * RotateZ(n);
	ngan();

	//banh xe 1
	instance_temp1 = Translate(0.1 + xxe,0.02+ yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(x, 0.0, -0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
	//banh xe 2
	instance_temp1 = Translate(0.1 + xxe, 0.02 + yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(x, 0.0, 0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
	//banh xe 3
	instance_temp1 = Translate(0.1 + xxe, 0.02 + yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(0.0, 0.0, -0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
	//banh xe 4
	instance_temp1 = Translate(0.1 + xxe, 0.02 + yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(0.0, 0.0, 0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
	//banh xe 5
	instance_temp1 = Translate(0.1 + xxe, 0.02 + yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(-x, 0.0, -0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
	//banh xe 6
	instance_temp1 = Translate(0.1 + xxe, 0.02 + yxe, zxe) * RotateX(xoay[0]) * RotateY(xoay[1]) * RotateZ(xoay[2]);
	instance_temp1 *= Translate(-x, 0.0, 0.6 * z);
	obanhxe();
	for (int i = 1; i <= 360; i++) {
		instance_temp1 *= Translate(0.0, 0.0, 0.0) * RotateZ(i);
		Banhxe();
	}
}

// maycau cau
mat4 instance_mc;
mat4 model_mc;
mat4 model_view_mc;
mat4  instance_mc1;
GLfloat xoaycau[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
void banhmaycau()
{
	material_diffuse = vec4(0, 0.2, 0.2, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_mc = Translate(0, -0.02, -0.2) * Scale(0.9, 0.08, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance_mc = Translate(0, -0.02, 0.2) * Scale(0.9, 0.08, 0.15);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void buongdieukhien()
{
	material_diffuse = vec4(0, 0.2, 0.2, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	// Bệ ngồi
	instance_mc = Translate(-0.1, 0.08, 0) * Scale(0.4, 0.03, 0.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// mái che
	instance_mc = Translate(-0.1, 0.2, 0) * Scale(0.4, 0.01, 0.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	// cột mái che
	instance_mc = Translate(0.09, 0.14, 0.19) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance_mc = Translate(0.09, 0.14, -0.19) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance_mc = Translate(-0.29, 0.14, -0.19) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance_mc = Translate(-0.29, 0.14, 0.19) * Scale(0.02, 0.12, 0.02);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void thanmaycau()
{
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_mc = Translate(0, 0.04, 0) * Scale(0.6, 0.05, 0.4);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	
}


void cancau1()
{
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_mc = RotateZ(-50) * Scale(0.07, 0.35, 0.07);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cancau2()
{
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_mc = RotateZ(-100) * Scale(0.07, 0.45 ,0.07);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cancau3()
{
	material_diffuse = vec4(0, 0.2, 0.2, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_mc = RotateZ(-100) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_mc * model_view_mc * instance_mc1 * instance_mc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void maycau()
{
	instance_mc1 = Translate(a-0.9, 0.05, 0.4) * RotateX(xoaycau[0]) * RotateY(xoaycau[1]) * RotateZ(xoaycau[2]);
	banhmaycau();
	instance_mc1 = instance_mc1 *RotateY(xoaycau[3]);
	thanmaycau();
	buongdieukhien();
	instance_mc1 = instance_mc1 * Translate(0.25, 0, 0)* RotateZ(xoaycau[4]) * Translate(0.15, 0.135, 0);
	cancau1();
	instance_mc1 = instance_mc1  * Translate(0.18, 0.08, 0) * RotateZ(xoaycau[5]) * Translate(0.16, 0,0);
	cancau2();
	instance_mc1 = instance_mc1  * Translate(0.2, -0.06, 0) * RotateZ(xoaycau[6]) * Translate(0.05, 0, 0);
	cancau3();
}
///////////////
mat4 instance_cc;
mat4 model_cc;
mat4 model_view_cc;
mat4  instance_cc1;
GLfloat dkhiencc[] = { 0, 0, 0, 0 };

void chanDe() {
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0) * Scale(0.3, 0.1, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}


void thanCanCau() {
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0) * Scale(0.07, 2.0, 0.07);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void buongDK() {
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0) * Scale(0.3, 0.1, 0.3);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void tayCanCau() {
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0)* Translate(0.5, 0, 0) * Scale(2.0, 0.07, 0.07);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc  * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void day() {
	material_diffuse = vec4(0, 0, 0, 1);   // mau den
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0)* Translate(0, -(0.2 + dkhiencc[2])/2, 0) * Scale(0.01, 0.2 + dkhiencc[2], 0.01);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc  * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void khoiGachCauLen() 
{
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_cc = RotateZ(0) *  Scale(0.1,0.1,0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_cc * instance_cc1 * instance_cc);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void canCau() {

	instance_cc1 = Translate(1.0, 0.05, -0.9);
	chanDe();
	instance_cc1 = instance_cc1 * Translate(0,0.05,0) * RotateX(0) * RotateY(0) * RotateZ(0);
	thanCanCau();
	instance_cc1 = instance_cc1 *Translate(0,1-0.2 + dkhiencc[0],0)* RotateY(dkhiencc[1]);
	tayCanCau();
	instance_cc1 = instance_cc1 * Translate(-0.2, 0.035 + 0.025 * 2, 0) * RotateY(0);
	buongDK();
	instance_cc1 = instance_cc1 * Translate(1.5 + dkhiencc[3], -0.1, 0);
	day();
	instance_cc1 = instance_cc1 * Translate(0, -0.2-dkhiencc[2], 0);
	khoiGachCauLen();


}

//Nhà xây dở
mat4 instance_nxd;
mat4 instance_nxd1;
mat4 model_nxd;
mat4 model_view_nxd;


void cotNha() {
	material_diffuse = vec4(0.5, 0.5, 0.5, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_nxd = RotateZ(0) * Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nxd * instance_nxd1 * instance_nxd);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void sanNha() {
	material_diffuse = vec4(0.5, 0.5, 0.5, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_nxd = RotateZ(0) * Scale(1, 0.1, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_nxd * instance_nxd1 *instance_nxd);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void nhaXayDo() {

	instance_nxd1 = Translate(1.4, 0.5/2, -0.9);
	cotNha();
	instance_nxd1 = instance_nxd1 * Translate(1, 0, 0);
	cotNha();
	instance_nxd1 = instance_nxd1 * Translate(0, 0, 1);
	cotNha();
	instance_nxd1 = instance_nxd1 * Translate(-1, 0, 0);
	cotNha();
	instance_nxd1 = instance_nxd1 * Translate(+0.5, 0.05, -(0.5));
	sanNha();
}
//dong gach
mat4 instance_dg;
mat4 instance_dg1;
mat4 model_dg;
mat4 model_view_dg;


void dongGach() {
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_dg = RotateZ(0) * Scale(0.1, 0.1, 0.1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dg * instance_dg1 * instance_dg);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void dongGachMain() {
	instance_dg1 = Translate(1.4, 0.5, 0.9);
	dongGach();
	instance_dg1 = Translate(1.6, 0.5, 0.7);
	dongGach();
	instance_dg1 = Translate(1.6, 0.5, 0.9);
	dongGach();

}


mat4 instance_bb;
mat4 instance_bb1;
mat4 model_bb;

void cotBienBao() {
	material_diffuse = vec4(0, 0, 0, 1);   // mau den
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_bb = RotateZ(0) * Scale(0.01, 0.05, 0.01);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_bb * instance_bb1 * instance_bb);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void noiDungBien() {
	material_diffuse = vec4(0, 0, 1, 1);   // mau xanh
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_bb = RotateZ(0) * Scale(0.01, 0.1, 0.12);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_bb * instance_bb1 * instance_bb);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}



void bienBao() {
	instance_bb1 = Translate(2.4, 0.5, 0.7);
	cotBienBao();
	instance_bb1 = instance_bb1 * Translate(0, 0, 0.08);
	cotBienBao();

	instance_bb1 = instance_bb1 * Translate(0, 0.05 +0.025, -0.04);
	noiDungBien();
}


mat4 instance_dc;
mat4 instance_dc1;
mat4 model_dc;
mat4 model_view_dc;


void dongCat() {
	material_diffuse = vec4(1, 1, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	instance_dc = RotateY(0) * Scale(0.5, 0.5, 0.5);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_dc * instance_dc1 * instance_dc);
	glDrawArrays(GL_TRIANGLES, NumPoints +1, NumPoints+17);
}

void dongCatMain() {
	instance_dc1 = Translate(2.0, 0.5, 0.9);
	dongCat();
	

}

//kich thuoc thung xe
GLfloat daiThung = 0.6;
GLfloat rongThung = 0.35;
GLfloat caoThung = 0.3;

//kich thuoc dau xe
GLfloat daiDau = 0.2;
GLfloat rongDau = 0.35;
GLfloat caoDau = 0.25;

//kich thuoc banh xe
GLfloat daiBanh = 0.1;
GLfloat rongBanh = 0.04;
GLfloat caoBanh = 0.1;

mat4 ctm;//lay hanh dong cua cac doi tuong
mat4 quay;//quay ca xe
GLfloat tien = -0.9;

GLfloat thetaXebon[] = { 0,0,0 };
GLfloat radius = 1.0, thet = 0.0, phi = 0.0;
GLfloat dr = 5.0 * DegreesToRadians;

void veXeBon(GLfloat x, GLfloat y, GLfloat z, mat4 mt) {
	point4 eye(radius * sin(thet) * cos(phi),
		radius * sin(thet) * sin(phi),
		radius * cos(thet), 1.0);
	point4 at(0, 0, 0, 1.0);
	vec4 up(0, 1, 0, 1.0);
	mat4 v = LookAt(eye, at, up);
	mat4 ins = Scale(x, y, z);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, v * quay * mt * ins);
	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat moThung = 0;

void thungXe(mat4 m) {
	material_diffuse = vec4(1, 1, 0, 1);   // mau vang
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);

	ctm = Translate(0, 0, 0) * m;
	for (GLint i = 0; i < 180; i++) {
		veXeBon(daiThung, 0.01, rongThung, ctm * RotateX(i));
	}

}

void cuaxe(mat4 mt) {
	material_diffuse = vec4(1, 0, 0, 1);   // mau do
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	//phanduoi
	ctm = mt * Translate(-daiDau / 2, 0, 0);
	veXeBon(daiDau, caoDau * 2 / 3, 0.02, ctm);
	//phantrenphai
	ctm = mt * Translate(0, ((caoDau / 3) / 2) + ((caoDau * 2 / 3) / 2), 0) * Translate(-0.005, 0, 0) ;
	veXeBon(0.01, caoDau * 1 / 3, 0.02, ctm);
	//phantrentrai
	ctm = mt * Translate(-daiDau + 0.01, ((caoDau / 3) / 2) + ((caoDau * 2 / 3) / 2), 0) * Translate(-0.005, 0, 0);
	veXeBon(0.01, caoDau * 1 / 3, 0.02, ctm);
	//phantrencung
	ctm = mt * Translate(0, ((caoDau / 3)) + ((caoDau * 2 / 3) / 2) + 0.005, 0) * Translate(-daiDau / 2, 0, 0);
	veXeBon(daiDau, 0.01, 0.02, ctm);

}
mat4 door;
GLfloat moCua = 0;
void dauXe(mat4 m) {
	//daydau
	ctm = Translate((daiThung / 2) + (daiDau / 2), (-caoThung / 2) + 0.01, 0) * m;
	veXeBon(daiDau, 0.02, rongDau, ctm);
	//cuatren
	door = Translate(daiThung / 2 + daiDau, -(caoDau * 2 / 3) / 2 + (0.038), rongDau / 2 - 0.01) * RotateY(moCua) * m;
	cuaxe(door);
	//cuaduoi
	door = Translate(daiThung / 2 + daiDau, -(caoDau * 2 / 3) / 2 + (0.038), -rongDau / 2 + 0.01) * RotateY(-moCua) * m;
	cuaxe(door);
	//noc
	ctm = Translate((daiThung / 2) + (daiDau / 2), (caoThung / 2) - 0.01, 0) * m;
	veXeBon(daiDau, 0.02, rongDau, ctm);
	//matxe
	ctm = Translate(daiThung / 2 + daiDau + 0.01, (-rongThung / 2) + (caoDau * 4 / 9), 0) * m;
	veXeBon(0.02, caoDau * 2 / 3, rongDau, ctm);

	ctm = Translate(daiThung / 2 + daiDau + 0.01, (rongThung / 2) - (caoDau / 3) - 0.02, (rongDau / 2) - 0.03) * m;
	veXeBon(0.02, caoDau / 3 + 0.03, 0.03, ctm);

	ctm = Translate(daiThung / 2 + daiDau + 0.01, (rongThung / 2) - (caoDau / 3) - 0.02, -(rongDau / 2) + 0.03) * m;
	veXeBon(0.02, caoDau / 3 + 0.03, 0.03, ctm);
}
void chanbun(mat4 mt) {
	material_diffuse = vec4(0, 0.2, 0.2, 1);   // mau xam
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	veXeBon(0.01, 0.05, 0.04, mt * RotateZ(-30) * Translate(-0.1, -0.02, 0));
	veXeBon(0.01, 0.05, 0.04, mt * RotateZ(30) * Translate(0.1, -0.02, 0));
}

void banhXeTrenTrai(mat4 m) {

	chanbun(Translate(0.35, -0.22, -0.25) * m);
	ctm = Translate(0.35, -0.22, -0.25) * m;
	for (GLint i = 0; i < 90; i++) {
		veXeBon(daiBanh, caoBanh, rongBanh, ctm * RotateZ(i));
	}
}
void banhXeTrenPhai(mat4 m) {

	chanbun(Translate(0.35, -0.22, 0.15) * m);
	ctm = Translate(0.35, -0.22, 0.15) * m;
	for (GLint i = 0; i < 90; i++) {
		veXeBon(daiBanh, caoBanh, rongBanh, ctm * RotateZ(i));
	}
}
void banhXeDuoiTrai(mat4 m) {

	chanbun(Translate(-0.35, -0.22, -0.35) * m);
	ctm = Translate(-0.35, -0.22, -0.35) * m;
	for (GLint i = 0; i < 90; i++) {
		veXeBon(daiBanh, caoBanh, rongBanh, ctm * RotateZ(i));
	}
}
void banhXeDuoiPhai(mat4 m) {

	chanbun(Translate(-0.35, -0.22, 0.05) * m);
	ctm = Translate(-0.35, -0.22, 0.05) * m;
	for (GLint i = 0; i < 90; i++) {
		veXeBon(daiBanh, caoBanh, rongBanh, ctm * RotateZ(i));
	}
}
GLfloat xxebon = 0;
void Xe() {
	mat4 move = Translate(-0.9 + xxebon, 0, 0);
	thungXe(move);
	dauXe(move);
	banhXeTrenTrai(move);
	banhXeTrenPhai(move);
	banhXeDuoiTrai(move);
	banhXeDuoiPhai(move);
}




void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );  
	veSan();
	xe();
	canCau();
	maycau();
	nhaXayDo();
	dongGachMain();
	bienBao();
	//dongCatMain();
	quay = Translate(0, 0.3, 0.8) ;
	Xe();
	

	vec4 eye(eye_x, eye_y, eye_z, 1);
	vec4 at(1, 0, 0, 1);
	vec4 up(0, 1, 0, 1);

	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	mat4 projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
	glutSwapBuffers();									   
}

void rotate360(GLfloat& x) {
	x += 5;
	if (x + 5 >= 360) x -= 360;
	glutPostRedisplay();
}

void rotate_360(GLfloat& x) {
	x -= 5;
	if (x - 5 <= 360) x += 360;
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
		//dk xe tai
	case 'a': xoay[0] += 5;//xoay theo chieu x
		glutPostRedisplay();
		break;
	case 'A': xoay[0] -= 5;
		glutPostRedisplay();
		break;
	case 's': xoay[1] += 3;//xoay theo chieu y
		glutPostRedisplay();
		break;
	case 'S': xoay[1] -= 3;
		glutPostRedisplay();
		break;
	case 'd': xoay[2] += 3;//xoay theo chieu z
		glutPostRedisplay();
		break;
	case 'D': xoay[2] -= 3;
		glutPostRedisplay();
		break;
	case 'f': m += 5; // dk thung xe
		n -= 8;
		if (n <= -60) n = -60;
		if (m >= 50) m = 50;
		glutPostRedisplay();
		break;
	case 'F': m -= 5;
		n += 8;
		if (n >= 0) n = 0;
		if (m < 0) m = 0;
		glutPostRedisplay();
		break;
	//case 'g': transX[0] += 0.05;//di chuyen
	//	if (transX[0] >= 0.5) transX[0] = 0.5;
	//	glutPostRedisplay();
	//	break;
	//case 'G': transX[0] -= 0.05;
	//	if (transX[0] <= -0.4) transX[0] = -0.4;
	//	glutPostRedisplay();
	//	break;
	case 'g': xxe += 0.05;//di chuyen
		if (xxe >= 0.1) xxe = 0.1;
		glutPostRedisplay();
		break;
	case 'G': xxe -= 0.05;
		if (xxe <= -0.9) xxe = -0.9;
		glutPostRedisplay();
		break;
	case '1': xoay[3] += 3;//xoay theo truc x
		glutPostRedisplay();
		break;
	case '2': xoay[4] += 5;//xoay theo truc y
		glutPostRedisplay();
		break;
		//dk thu phong
	case 'x': l *= 1.1f; r *= 1.1f;
		glutPostRedisplay();
		break;
	case 'X': l *= 0.9f; r *= 0.9f;
		glutPostRedisplay();
		break;
	case 'y': bottom *= 1.1f; top *= 1.1f;
		glutPostRedisplay();
		break;
	case 'Y': bottom *= 0.9f; top *= 0.9f;
		glutPostRedisplay();
		break;
	case 'z': zNear *= 1.1f; zFar *= 1.1f;
		glutPostRedisplay();
		break;
	case 'Z': zNear *= 0.9f; zFar *= 0.9f;
		glutPostRedisplay();
		break;

	case ' ': // reset values to their defaults
		l = -0.75;
		r = 0.75;
		bottom = -0.75;
		top = 0.75;
		zNear = 1;
		zFar = 3;
		glutPostRedisplay();
		break;

	case '4': eye_x += 0.2;
		glutPostRedisplay();
		break;
	case '5': eye_y += 0.2;
		glutPostRedisplay();
		break;
	case '6': eye_z += 0.2;
		glutPostRedisplay();
		break;
	case '7': eye_x -= 0.2;
		glutPostRedisplay();
		break;
	case '8': eye_y -= 0.2;
		glutPostRedisplay();
		break;
	case '9': eye_z -= 0.2;
		glutPostRedisplay();
		break;
		// DK MAY CAU
	case 'U': a -= 0.01;
		glutPostRedisplay();
		break;
	case 'u': a += 0.01;
		glutPostRedisplay();
		break;
	case 'L': xoaycau[0] += 5;//xoay theo chieu x
		glutPostRedisplay();
		break;
	case 'l': xoaycau[0] -= 5;
		glutPostRedisplay();
		break;
	case 'J': xoaycau[1] += 3;//xoay theo chieu y
		glutPostRedisplay();
		break;
	case 'j': xoaycau[1] -= 3;
		glutPostRedisplay();
		break;
	case 'I': xoaycau[2] += 3;//xoay theo chieu z
		glutPostRedisplay();
		break;
	case 'i': xoaycau[2] -= 3;
		glutPostRedisplay();
		break;
	case 'K': xoaycau[3] -= 3;
		glutPostRedisplay();
		break; 
	case 'P': xoaycau[4] -= 3;
		if (xoaycau[4] - 3 <= 0)  xoaycau[4] -= 0;
	glutPostRedisplay();
	 break;
	case 'p': xoaycau[4] += 3;
		glutPostRedisplay();
		break;
	case 'N': xoaycau[5] += 3;//xoay theo chieu z
		glutPostRedisplay();
		break;
	case 'n': xoaycau[5] -= 3;
		glutPostRedisplay();
		break;
	case 'M': xoaycau[6] += 3;//xoay theo chieu z
		glutPostRedisplay();
		break;
	case 'm': xoaycau[6] -= 3;
		glutPostRedisplay();
		break;
	// dk xe bon
	case 'q':
		xxebon += 0.01;
		if (xxebon > 0.3) xxebon = 0.3;
		break;
	case 'Q':
		xxebon -= 0.01;
		if (xxebon < 0) xxebon = 0;
		break;
	case 'e':
		moCua += 5;
		if (moCua > 60) moCua = 60;
		break;
	case 'E':
		moCua -= 5;
		if (moCua < 0) moCua = 0;
		break;
	case 'c':
		dkhiencc[0] += 0.02;
		if (dkhiencc[0] >= 0.1) dkhiencc[0] = 0.1;
		break;
	case 'C':
		dkhiencc[0] -= 0.02;
		if (dkhiencc[0] <= -0.4) dkhiencc[0] = -0.4;
		break;
	case 'v':
		dkhiencc[1] += 5;
		break;
	case 'V':
		dkhiencc[1] -= 5;
		break;
	case 'b':
		dkhiencc[2] += 0.01;
		if (dkhiencc[2] - dkhiencc[1] >= 0.6) dkhiencc[2] = 0.6;
		break;
	case 'B':
		dkhiencc[2] -= 0.01;
		if (dkhiencc[2]<= -0.05) dkhiencc[2] = -0.05;
		break;
	case ',':
		dkhiencc[3] += 0.01;
		if (dkhiencc[3]>= 0.16) dkhiencc[3] = 0.16;
		break;
	case '<':
		dkhiencc[3] -= 0.01;
		if (dkhiencc[3] <= -1) dkhiencc[3] = -1;
		break;
	
	}

	glutPostRedisplay();
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Cong Truong" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
