#pragma once

#include "userdata.h"
#include <data.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ktx.h>

#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define LOC_POSITION  0
#define LOC_NORMAL    1
#define LOC_TEXCOORD  2
#define LOC_TANGENT   3

const unsigned int SHADOWWIDTH = 4096, SHADOWHEIGHT = 4096;
#define MAX_LIGHT 16

typedef struct{
    GLuint vao, ebo; // ebo for indices

    GLuint* vbos;
    size_t vbos_count;

    GLenum draw_mode; // for gl_triangles, and etc.

    size_t index_count;
    GLenum index_type;
    GLuint albedomap,normalmap,ormmap,emissivemap;
    mat4 transform;
} meshdata;

typedef struct{
    vec3 pos;
    vec3 col;
    float inten;
    float range;
    int type;
} light;

//Data for global models
typedef struct{
    meshdata* primitives;
    int count;

    light lights[MAX_LIGHT];
    int lightcount;

    mat4 lightspacematrix;

    mat4* instances;

    
} model;

//Data for global materials
typedef struct {
    char name[64];
    GLuint albedomap;
    GLuint normalmap;
    GLuint ormmap;
    GLuint emissivemap;

    float basecolor[4];
    float metallic;
    float roughness;
}material;

/* These structs are for nanite like feature. Currently paused
typedef struct{
    float transform[16];
    float normalmat[16];
} Instancesdata;

typedef struct{
    float center[3], radius;
    float error, parenterror;
    uint32_t offset, count;
    uint32_t meshid, pad;
} Cluster;

typedef struct{
    uint32_t count;
    uint32_t instancecount;
    uint32_t firstindex;
    uint32_t basevertex;
    uint32_t baseinstance;
} Drawcommand;
*/

GLuint clusterbuf, commandbuf, counterbuf;

//For converting cgltf types into gl's understandable ones
GLenum mapcomponenttype(cgltf_component_type type){
    switch (type) {
        case cgltf_component_type_r_8:   return GL_BYTE;
        case cgltf_component_type_r_8u:  return GL_UNSIGNED_BYTE;
        case cgltf_component_type_r_16:  return GL_SHORT;
        case cgltf_component_type_r_16u: return GL_UNSIGNED_SHORT;
        case cgltf_component_type_r_32u: return GL_UNSIGNED_INT;
        case cgltf_component_type_r_32f: return GL_FLOAT;
        default: return GL_FLOAT;
    }
}
//For converting KTX types into gl's understandable ones
GLenum GlCompressedFormat(ktx_transcode_fmt_e target, int srgb){
    switch (target) {
        case KTX_TTF_BC7_RGBA:
            return srgb ? GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM : GL_COMPRESSED_RGBA_BPTC_UNORM;
        case KTX_TTF_BC3_RGBA:
            return srgb ? GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        case KTX_TTF_ASTC_4x4_RGBA:
            return srgb ? GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR : GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        default:
            return 0;
    }
}
GLuint shadowfbo, shadowmap;

void initshadow(){
    glGenFramebuffers(1, &shadowfbo);

    glGenTextures(1, &shadowmap);
    glBindTexture(GL_TEXTURE_2D, shadowmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWWIDTH, SHADOWHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float bordercolor[] = {1.0f,1.0f,1.0f,1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowfbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowmap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


// For converting path into understandable string
void ResURIPath(char* Pathout, const char* model, const char* URI){
    const char* lastslash = strrchr(model, '/');

    if (!lastslash) {
        lastslash = strrchr(model, '\\');
    }

    if (lastslash) {
        size_t dirlen = (lastslash - model) + 1;
        strncpy(Pathout, model, dirlen);
        Pathout[dirlen] = '\0';
        strcat(Pathout, URI);

    }else {
        strcat(Pathout, URI);
    }
}
// For transcoding textures with KTX feature
GLuint TranscodeKTX2(cgltf_image image, const char* model, int srgb){
    ktxTexture* texture  = NULL;
    KTX_error_code result;

    if (image.uri) {
        char fullpath[1024] = {0};
        ResURIPath(fullpath, model, image.uri);
        result = ktxTexture_CreateFromNamedFile(fullpath, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture);
    }else if (image.buffer_view) {
        uint8_t* pointer = (uint8_t*)image.buffer_view->buffer->data + image.buffer_view->offset;
        result = ktxTexture_CreateFromMemory(pointer,image.buffer_view->size, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture);
    }else {
        return 0;
    }

    if (result != KTX_SUCCESS){
        printf("Failed to parse KTX2 | %s\n", ktxErrorString(result));
        return 0;
    }
    ktx_transcode_fmt_e targetformat = KTX_TTF_BC7_RGBA;

    if (ktxTexture_NeedsTranscoding(texture)) {

        result = ktxTexture2_TranscodeBasis((ktxTexture2*)texture, targetformat, 0);

        if (result != KTX_SUCCESS) {
            printf("Failed to transcode KTX2 | %s\n", ktxErrorString(result));
            ktxTexture_Destroy(texture);
            return 0;
        }

        printf("KTX2 Transcoded\n");
    }

    GLenum GLformat = GlCompressedFormat(targetformat, srgb);

    GLuint gltex;
    glGenTextures(1, &gltex);
    glBindTexture(GL_TEXTURE_2D, gltex);

    for (uint32_t level = 0; level < texture->numLevels; level++) {
        size_t offset = 0; 
        ktxTexture_GetImageOffset(ktxTexture(texture), level, 0, 0, &offset);

        uint32_t mip_width = texture->baseWidth >> level;
        uint32_t mip_height = texture->baseHeight >> level;

        if (mip_width == 0) {
            mip_width = 1;
        }

        if (mip_height == 0) {
            mip_height = 1;
        }
        size_t mipsize = ktxTexture_GetImageSize(ktxTexture(texture), level);
        unsigned char* mipdata = ktxTexture_GetData(ktxTexture(texture)) + offset;

        glCompressedTexImage2D(GL_TEXTURE_2D,level,GLformat, mip_width, mip_height, 0, (GLsizei)mipsize, mipdata);

    }
    ktxTexture_Destroy(ktxTexture(texture));

    return gltex;
}
// For checking if KTX actually exist as a feature
cgltf_image* GetImgExtension(cgltf_texture* texture){
    if (!texture) {
        return NULL;
    }

    if (texture->has_basisu) {
        return texture->basisu_image;
    }

    return texture->image;
}

// Just a Sampler for the textures
void GLApplySampler(cgltf_texture* texture){
    GLint minfilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint magfilter = GL_LINEAR;
    GLint wraps = GL_REPEAT;
    GLint wrapt = GL_REPEAT;

    if (texture->sampler) {
        if (texture->sampler->min_filter) {
            minfilter = texture->sampler->min_filter;
        }
        if (texture->sampler->mag_filter) {
            magfilter = texture->sampler->mag_filter;
        }
        if (texture->sampler->wrap_s) {
            wraps = texture->sampler->wrap_s;
        }
        if (texture->sampler->wrap_t) {
            wrapt = texture->sampler->wrap_t;
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    }
}
// For loading materials from GLTF data
material* InitMaterial(cgltf_data* data, const char* modpath, int* out){
    if(!data) return NULL;

    *out = (int)data->materials_count;
    material* materials = (material*)malloc(sizeof(material) * data->materials_count);

    for (cgltf_size i = 0; i < data->materials_count; ++i){
        cgltf_material* mat = &data->materials[i];
        material* glmat = &materials[i];

        strncpy(glmat->name, mat->name ? mat->name : "Unnamed", 63);
        glmat->albedomap = glmat->normalmap = glmat->ormmap = glmat->emissivemap = 0;
        glmat->roughness = mat->has_pbr_metallic_roughness ? mat->pbr_metallic_roughness.roughness_factor : 0.5f;
        glmat->metallic = mat->has_pbr_metallic_roughness ? mat->pbr_metallic_roughness.metallic_factor : 0.0f;

        if (mat->has_pbr_metallic_roughness) {
            cgltf_texture* texture = mat->pbr_metallic_roughness.base_color_texture.texture;
            cgltf_image* image = GetImgExtension(texture);
            if (image) {
                glmat->albedomap = TranscodeKTX2(*image, modpath, 1);
                if (glmat->albedomap) {
                    glBindTexture(GL_TEXTURE_2D, glmat->albedomap);
                    GLApplySampler(texture);
                }

            }
        }
        if (mat->normal_texture.texture) {
            cgltf_texture* texture = mat->normal_texture.texture;
            cgltf_image* image = GetImgExtension(texture);
            if (image) {
            glmat->normalmap = TranscodeKTX2(*image, modpath, 0);
                if (glmat->normalmap){
            glBindTexture(GL_TEXTURE_2D, glmat->normalmap);
            GLApplySampler(texture);
            }
        }
        }

        if(mat->has_pbr_metallic_roughness){
            cgltf_texture* texture = mat->pbr_metallic_roughness.metallic_roughness_texture.texture;
            cgltf_image* image = GetImgExtension(texture);
                if (image) {
                glmat->ormmap = TranscodeKTX2(*image, modpath, 0);
                    if (glmat->ormmap) {
                    glBindTexture(GL_TEXTURE_2D, glmat->ormmap);
                    GLApplySampler(texture);
                    }
            }
        }

        if (mat->emissive_texture.texture) {
            cgltf_texture* texture = mat->emissive_texture.texture;
            cgltf_image* image = GetImgExtension(texture);
                if (image) {
                glmat->emissivemap = TranscodeKTX2(*image, modpath, 1);
                    if (glmat->emissivemap) {
                    glBindTexture(GL_TEXTURE_2D, glmat->ormmap);
                    GLApplySampler(texture);
                    }
                }
        }
    }

return materials;
}

// Just some random function. It was used for applying Mesh coordinated into mutable variable
void MeshOperator4fv(GLuint program, GLint uniform, mat4 Model, vec3 Position, float scale){
    glm_mat4_identity(Model);
    glm_translate(Model, Position);
    glm_scale_uni(Model, scale);
    GLint Uniform = glGetUniformLocation(program, "model");
    glUniformMatrix4fv(uniform, 1, GL_FALSE, (float*)Model);
}
// This code was used for rendering randomly generated flat using perlin-like noise.  
model InitMesh(Vertex* vertices, int vc, uint32_t *indices, int ic){
    model m;
    m.count = 1;
    m.primitives = (meshdata*)malloc(sizeof(meshdata) * m.count);

    m.primitives[0].index_count = ic;
    m.primitives[0].index_type = GL_UNSIGNED_INT;
    m.primitives[0].draw_mode = GL_TRIANGLES;

    m.primitives[0].vbos_count = 1;
    m.primitives[0].vbos = (GLuint*)malloc(sizeof(GLuint) * 1);

    glGenVertexArrays(1, &m.primitives[0].vao);
    glGenBuffers(1,&m.primitives[0].ebo);
    glGenBuffers(1,m.primitives[0].vbos);

    glBindVertexArray(m.primitives[0].vao);

    glBindBuffer(GL_ARRAY_BUFFER, m.primitives[0].vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vc, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.primitives[0].ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * ic, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,3 * sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,3 *sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    if(m.primitives[0].vao != 0){
        return m;
    }
    printf("model init error");
}

// Name is telling
model LoadMesh(const char* filepath) {
    model map = {0};
    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));

    options.type = cgltf_file_type_invalid;

    cgltf_data* data = NULL;
    options.json_token_count = 0; // Let the library determine the token count
    
    cgltf_result result = cgltf_parse_file(&options, filepath, &data); // Parse the glTF file
    
    if (result != cgltf_result_success) {
        fprintf(stderr, "Failed to load glTF file: %s\n", filepath);
        return (model){0};
    }

    if (cgltf_load_buffers(&options, data, filepath) != cgltf_result_success) {
        fprintf(stderr, "Failed to load buffers for glTF file: %s\n", filepath);
        cgltf_free(data);
        return (model){0};
    }

    //counts all primitives from file
    int total_primitives = 0;
    for (size_t i = 0; i < data->nodes_count; i++ ) {
        if (data->nodes[i].mesh) {
            total_primitives += data->nodes[i].mesh->primitives_count;
        }
    }

    int total_materials = 0;
    material* loadedmaterials = InitMaterial(data, filepath, &total_materials);

    if (total_primitives == 0) {
        printf("TOTAL PRIMS ARE 0!");
        return (model){0};
    }

    //meshdata* primitives = (meshdata*)calloc(total_primitives, sizeof(meshdata));
    int prim_index = 0;

    map.primitives = (meshdata*)calloc(total_primitives, sizeof(meshdata));
    map.count = total_primitives;
    map.lightcount = 0;

    for (size_t i = 0; i < data->nodes_count; ++i) {
        cgltf_node* node = &data->nodes[i];

        mat4 localtrans;
        cgltf_node_transform_world(node, (float*)localtrans);

        if (node->mesh) {
    
        for (size_t j = 0; j < node->mesh->primitives_count; ++j) {
            cgltf_primitive* primitive = &node->mesh->primitives[j];
            meshdata* m = &map.primitives[prim_index++];

            glm_mat4_copy(localtrans, m->transform);

            if (primitive->material) {
                int mat_idx = primitive->material - data->materials;
                m->albedomap = loadedmaterials[mat_idx].albedomap;
                m->normalmap = loadedmaterials[mat_idx].normalmap;
                m->ormmap = loadedmaterials[mat_idx].ormmap;
                m->emissivemap = loadedmaterials[mat_idx].emissivemap    ;
            }
            
            glGenVertexArrays(1, &m->vao);
            glBindVertexArray(m->vao);

            size_t max_vbos = primitive->attributes_count + (primitive->indices ? 1 : 0);

            // Allocating an array of VBOs
            m->vbos = (GLuint*)calloc(max_vbos, sizeof(GLuint));
            m->vbos_count = max_vbos;
            int vbo_idx = 0;

            if (primitive->indices) {
                m->index_count = primitive->indices->count;
                m->index_type = mapcomponenttype(primitive->indices->component_type);

                glGenBuffers(1, &m->ebo);
                m->vbos[vbo_idx++] = m->ebo;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);

                cgltf_buffer_view* view = primitive->indices->buffer_view;


                void* data_ptr = (char*)view->buffer->data + view->offset + primitive->indices->offset;

                glBufferData(GL_ELEMENT_ARRAY_BUFFER, view->size, data_ptr, GL_STATIC_DRAW);

            }else {
            m->index_count = 0;
            }
            
            for (size_t k = 0; k < primitive->attributes_count; ++k) {
                cgltf_attribute* attr = &primitive->attributes[k];
                cgltf_accessor* acc = attr->data;
                cgltf_buffer_view* view = acc->buffer_view;

                //Shader thingy

                int location = -1;

                switch (attr->type) {
                    case cgltf_attribute_type_position:  location = LOC_POSITION; break;
                    case cgltf_attribute_type_normal:    location = LOC_NORMAL; break;
                    case cgltf_attribute_type_texcoord:  location = LOC_TEXCOORD; break;
                    case cgltf_attribute_type_tangent:   location = LOC_TANGENT; break;
                    default: break;
                }

                if (location != -1)
                 {
                    GLuint vbo;
                    glGenBuffers(1, &vbo);
                    m->vbos[vbo_idx++] = vbo;
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);

                    size_t accessor_size = acc->count * cgltf_calc_size(acc->type, acc->component_type);
                    void* data_ptr = (char*)view->buffer->data + view->offset + acc->offset;

                    glBufferData(GL_ARRAY_BUFFER, view->size, data_ptr, GL_STATIC_DRAW);

                    glEnableVertexAttribArray(location);

                    //determine vec2-4
                    int count = 0;
                    if (acc->type == cgltf_type_vec2) count = 2;
                    if (acc->type == cgltf_type_vec3) count = 3;
                    if (acc->type == cgltf_type_vec4) count = 4;
                    if (acc->type == cgltf_type_scalar) count = 1;

                    glVertexAttribPointer(location, count, mapcomponenttype(acc->component_type), acc->normalized, acc->stride, (void*)0);

                 }       
            }

        }}
        if (node->light != NULL && map.lightcount < 32) {
            light* l = &map.lights[map.lightcount++];
            
            mat4 wrdmat;
            
            cgltf_node_transform_world(node, (float*)wrdmat);
            l->pos[0] = wrdmat[3][0];
            l->pos[1] = wrdmat[3][1];
            l->pos[2] = wrdmat[3][2];
                        
            if (node->light) {
                glm_vec3_copy(node->light->color, l->col);
                l->inten = node->light->intensity * 0.001;
            }
            
        }
        if (node->camera) {
        
        }
    }
    glBindVertexArray(0);
    cgltf_free(data);
    free(loadedmaterials);

    return map;
}


//void modelapply(model* m){
//    glm_vec3_zero(m->position);
//    glm_vec3_one(m->scale);
//    glm_quat_identity(m->rotation);
//    glm_mat4_identity(m->modelmatrix);
//}

//void modelupdate(model* m){   
//    glm_mat4_identity(m->modelmatrix);
//
//    glm_translate(m->modelmatrix, m->position);
//
//    mat4 rotationmat;
//    glm_quat_mat4(m->position, rotationmat);
//    glm_mat4_mul(m->modelmatrix, rotationmat, m->modelmatrix);
//    glm_scale(m->modelmatrix, m->scale);
//}


void DrawMesh(model* m, GLuint program) {
    glUniform1i(glGetUniformLocation(program, "u_LightCount"), m->lightcount);

    for (int i = 0; i < m->lightcount; ++i) {
        char buffer[64];
        sprintf(buffer, "u_Lights[%d].position" , i);
        glUniform3fv(glGetUniformLocation(program, buffer), 1, m->lights[i].pos);
    
        sprintf(buffer, "u_Lights[%d].color" , i);
        glUniform3fv(glGetUniformLocation(program, buffer), 1, m->lights[i].col);
    
        sprintf(buffer, "u_Lights[%d].intensity" , i);
        glUniform1f(glGetUniformLocation(program, buffer), m->lights[i].inten);
    }

    GLint modelloc = glGetUniformLocation(program, "model");

    for (int i = 0; i < m->count; ++i) {
        meshdata* data = &m->primitives[i];
#ifdef NoRo_DEBUG_ON
        printf("DEBUG: Drawing Prim %d | VAO: %u | Indices: %zu\n", i, data->vao, data->index_count);
#endif
        glUniformMatrix4fv(modelloc, 1, GL_FALSE, (float*)data->transform);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, data->albedomap);
        glUniform1i(glGetUniformLocation(program, "u_AlbedoMap"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, data->normalmap);
        glUniform1i(glGetUniformLocation(program, "u_NormalMap"), 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, data->ormmap);
        glUniform1i(glGetUniformLocation(program, "u_ORMMap"), 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, data->emissivemap);
        glUniform1i(glGetUniformLocation(program,"u_EmissiveMap") , 3);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, shadowmap);
        glUniform1i(glGetUniformLocation(program,"u_ShadowMap") , 4);

        
        glBindVertexArray(data->vao);
        
        if (data->index_count > 0) {
            glDrawElements(GL_TRIANGLES, (GLsizei)data->index_count, data->index_type, 0);
        }

    }
    glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, (float*)m->primitives->transform);
    glUniformMatrix4fv(glGetUniformLocation(program, "u_LightSpaceMatrix"), 1, GL_FALSE, (float*)m->lightspacematrix);
    glBindVertexArray(0);
}

void CleanupMesh(model* m) {
    if (!m || !m->primitives) {
        return;
    }
    for (int i = 0; i< m->count; i++) {
        meshdata* mesh = &m->primitives[i];
        glDeleteTextures(1, &mesh->albedomap);
        glDeleteTextures(1, &mesh->normalmap);
        glDeleteTextures(1, &mesh->ormmap);
        glDeleteTextures(1, &mesh->emissivemap);

        if (mesh->vbos) {
            glDeleteBuffers((GLsizei)mesh->vbos_count, mesh->vbos);
            free(mesh->vbos);
            mesh->vbos = NULL;   
        }

        if (mesh->ebo) {
            glDeleteBuffers(1,&mesh->ebo);
        }

        if (mesh->vao) {
            glDeleteBuffers(1,&mesh->vao);
        }
    }
    if (m->primitives != NULL) {
        free(m->primitives);
        m->primitives = NULL;
    }
    m->count = 0;
}
