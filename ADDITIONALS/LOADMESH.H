#pragma once

#include <cstdio>
#include <strings.h>
#include <data.H>

#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>

#define LOC_POSITION  0
#define LOC_NORMAL    1
#define LOC_TEXCOORD  2
#define LOC_TANGENT   3
#define LOC_COLOR     4


typedef struct{
    GLuint vao, ebo; // ebo for indices

    GLuint* vbos;
    size_t vbos_count;

    GLenum draw_mode; // for gl_triangles, and etc.

    size_t index_count;
    GLenum index_type;
} meshdata;

typedef struct{
    meshdata* primitives;
    int count;
} model;

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

model LoadMesh(const char* filepath) {
    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));

    options.type = cgltf_file_type_invalid;
    
    options.json_token_count = 0; // Let the library determine the token count
    
    cgltf_data* data = NULL;
    
    cgltf_result result = cgltf_parse_file(&options, filepath, &data); // Parse the glTF file
    
    if (result != cgltf_result_success) { // Handle error
        fprintf(stderr, "Failed to load glTF file: %s\n", filepath);
        return (model){0};
    }

    if (cgltf_load_buffers(&options, data, filepath) != cgltf_result_success) {
        fprintf(stderr, "Failed to load buffers for glTF file: %s\n", filepath);
        cgltf_free(data);
        return (model){0};
    }

    

    //count all primitives in the file
    int total_primitives = 0;
    for (size_t i = 0; i < data->meshes_count; i++ ) {
        total_primitives += data->meshes[i].primitives_count;
    }

    meshdata* primitives = (meshdata*)calloc(total_primitives, sizeof(meshdata));
    int prim_index = 0;

    for (size_t i = 0; i < data->meshes_count; ++i) {
        cgltf_mesh* mesh = &data->meshes[i];

        for (size_t j = 0; j < mesh->primitives_count; ++j) {
            cgltf_primitive* primitive = &mesh->primitives[j];
            meshdata* m = &primitives[prim_index++];

            glGenVertexArrays(1, &m->vao);
            glBindVertexArray(m->vao);

            size_t max_vbos = primitive->attributes_count + (primitive->indices ? 1 : 0);

            // Allocating an array of VBOs : 1 for 1
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

                

                // SHADER LOCATION !!!!!!!

                int location = -1;

                switch (attr->type) {
                    case cgltf_attribute_type_position:  location = LOC_POSITION; break;
                    case cgltf_attribute_type_normal:    location = LOC_NORMAL; break;
                    case cgltf_attribute_type_texcoord:  location = LOC_TEXCOORD; break;
                    case cgltf_attribute_type_tangent:   location = LOC_TANGENT; break;
                    case cgltf_attribute_type_color:     location = LOC_COLOR; break;
                    default: break;
                }

                

                if (location != -1)
                 {
                    GLuint vbo;
                    glGenBuffers(1, &vbo);
                    m->vbos[vbo_idx++] = vbo;
                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    
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
            glBindVertexArray(0);

        }
    }


    cgltf_free(data);
    return (model){primitives, total_primitives};
}


void DrawMesh(model* m) {
    for (int i = 0; i < m->count; ++i) {
        meshdata* mesh = &m->primitives[i];

        if (mesh->vao == 0) {
            continue;
        }

        printf("DEBUG: Drawing Prim %d | VAO: %u | Indices: %zu\n", i, mesh->vao, mesh->index_count);

        
        if (mesh->index_count > 0) {
            GLenum err = 0;
            while ((err == glGetError()) != GL_NO_ERROR) {
                fprintf(stderr, "GL ERROR : 0x%x\n", err);
            }
            glBindVertexArray(mesh->vao);
            glDrawElements(GL_TRIANGLES, mesh->index_count, mesh->index_type, 0);
        } else {

        }
    }
    glBindVertexArray(0);
}

void CleanupMesh(model* m) {
    for (int i = 0; i < m->count; ++i) {
        meshdata* mesh = &m->primitives[i];
        if (mesh->vbos) {
            glDeleteBuffers(mesh->vbos_count, mesh->vbos);
            free(mesh->vbos);
        }
        if (mesh->vao) glDeleteVertexArrays(1, &mesh->vao);
    }
    if (m->primitives) {
        free(m->primitives);
    }
    m->primitives = NULL;
    m->count = 0;
}