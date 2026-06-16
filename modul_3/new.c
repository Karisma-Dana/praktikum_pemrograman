#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    int **data;
    int row;
    int column;
} Matrix;

typedef struct
{
    int x;
    int y;
    int z;
} Vector;

void allocation_mem(Matrix *matrix, int row, int column)
{
    matrix->row = row;
    matrix->column = column;
    matrix->data = (int **)malloc(matrix->row * sizeof(int *));
    for (int i = 0; i < matrix->row; i++)
    {
        matrix->data[i] = (int *)malloc(matrix->column * sizeof(int));
    }
}

void createMatrix(Matrix *matrix, int row, int column)
{

    allocation_mem(matrix, row, column);

    printf("\n===== INPUT MATRIX %i x %i ====\n", matrix->row, matrix->column);
    for (int i = 0; i < matrix->row; i++)
    {
        for (int j = 0; j < matrix->column; j++)
        {
            printf("\ninput matrix [%i][%i] = ", i, j);
            scanf("%i", &matrix->data[i][j]);
        }
    }
}
void displayMarix(Matrix matrix)
{
    printf("\nDISPLAY HASIL MATRIX\n");
    for (int i = 0; i < matrix.row; i++)
    {
        printf("|\t");
        for (int j = 0; j < matrix.column; j++)
        {
            printf("%i\t", matrix.data[i][j]);
        }
        printf("|\n");
    }
    printf("\n");
}

void addMatrix(Matrix mat_1, Matrix mat_2, Matrix *matResult)
{

    if ((mat_1.row == mat_2.row) && (mat_1.column == mat_2.column))
    {
        // allocation memory for matrix result base on row and collumn
        allocation_mem(matResult, mat_1.row, mat_1.column);

        for (int i = 0; i < matResult->row; i++)
        {
            for (int j = 0; j < matResult->column; j++)
            {
                matResult->data[i][j] = mat_1.data[i][j] + mat_2.data[i][j];
            }
        }
    }
    else
    {
        printf("\n sorry matrix ordo is not same");
        return;
    }
}
void matrix_multiplication(Matrix mat_1, Matrix mat_2, Matrix *matResult)
{
    if (mat_1.column == mat_2.row)
    {

        // allocation memory for matric result base on mat_1.row and mat_2.column
        allocation_mem(matResult, mat_1.row, mat_2.column);
        for (int i = 0; i < mat_1.row; i++)
        {
            for (int j = 0; j < mat_2.column; j++)
            {
                matResult->data[i][j] = 0;
                for (int k = 0; k < mat_1.column; k++)
                {
                    matResult->data[i][j] += mat_1.data[i][k] * mat_2.data[k][j];
                }
            }
        }
    }
    else
    {
        printf("\n note : row matrix 1 shoul be same with column matrix 2, try again");
        return;
    }
}

void matrixTranspose(Matrix matBase, Matrix *matResult)
{
    allocation_mem(matResult, matBase.column, matBase.row);

    for (int i = 0; i < matBase.row; i++)
    {
        for (int j = 0; j < matBase.column; j++)
        {
            matResult->data[j][i] = matBase.data[i][j];
        }
    }
}

void inverseMatrix(Matrix *mat)
{
    if (mat->row == mat->column)
    {
        int n = mat->row;
        double **aug = (double **)malloc(n * sizeof(double *));
        for (int i = 0; i < n; i++)
        {
            aug[i] = (double *)malloc(2 * n * sizeof(double));
            for (int j = 0; j < 2 * n; j++)
            {
                if (j < n)
                    aug[i][j] = mat->data[i][j];
                else
                    aug[i][j] = (i == (j - n)) ? 1.0 : 0.0;
            }
        }

        for (int i = 0; i < n; i++)
        {
            double pivot = aug[i][i];
            if (pivot == 0)
            { // asumsikan non-singular di sini
                printf("Matriks singular!\n");
                return;
            }
            for (int j = 0; j < 2 * n; j++)
                aug[i][j] /= pivot;
            for (int k = 0; k < n; k++)
            {
                if (k != i)
                {
                    double factor = aug[k][i];
                    for (int j = 0; j < 2 * n; j++)
                        aug[k][j] -= factor * aug[i][j];
                }
            }
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                mat->data[i][j] = aug[i][j + n];
        }

        for (int i = 0; i < n; i++)
            free(aug[i]);
        free(aug);
    }
    else
    {
        printf("\n jumlah row dan column harus sama");
    }
}

int innerProduct(Matrix *mat_1, Matrix *mat_2)
{
    if ((mat_1->row == mat_2->row) && (mat_1->column == mat_2->column))
    {
        int result = 0;
        for (int i = 0; i < mat_1->row; i++) {
            for (int j = 0; j < mat_1->column; j++) {
        // Kalikan elemen yang posisinya sama, lalu akumulasikan
                result += mat_1->data[i][j] * mat_2->data[i][j];
            }
        }
    return result;
    }
    else
    {
        printf("\n ordo matrix 1 and matrix 2 should be same");
        return 0;
    }
}
void addVector(Vector vec_1, Vector vec_2, Vector *vec_result)
{
    vec_result->x = vec_1.x + vec_2.x;
    vec_result->y = vec_1.y + vec_2.y;
    vec_result->z = vec_1.z + vec_2.z;
}

void subVector(Vector vec_1, Vector vec_2, Vector *vec_result)
{
    vec_result->x = vec_1.x - vec_2.x;
    vec_result->y = vec_1.y - vec_2.y;
    vec_result->z = vec_1.z - vec_2.z;
}
int dotProductVector(Vector vec_1, Vector vec_2)
{
    int result = ((vec_1.x * vec_2.x) + (vec_1.y * vec_2.y) + (vec_1.z * vec_2.z));
    return result;
}

void crossProductVector(Vector vec_1, Vector vec_2, Vector *vec_result)
{
    // use determain metodh

    vec_result->x = (vec_1.y * vec_2.z) - (vec_1.z * vec_2.y);
    vec_result->y = -((vec_1.x * vec_2.z) - (vec_1.z * vec_2.x));
    vec_result->z = (vec_1.x * vec_2.y) - (vec_1.y * vec_2.x);
}

void createVector(Vector *vector)
{
    printf("\nmasukan nilai x = ");
    scanf("%i", &vector->x);
    printf("\nMasukan nilai y = ");
    scanf("%i", &vector->y);
    printf("\nMasukan nilai z = ");
    scanf("%i", &vector->z);
}

void displayVector(Vector vector)
{
    printf("\n nilai vector = %ix %iy %iz", vector.x, vector.y, vector.z);
}

int main()
{
    Matrix matrix_1, matrix_2, matrix_result;
    Vector vector_1, vector_2, vector_result;

    int input, row, column, row2, column2, result;
    int result_vector;
    bool isRunning = true;
    while (isRunning)
    {
        printf("\n\n===== WELOME TO MATH PROJECT =====\n");
        printf("1. Penjumlahan Matrix\n2. Perkalian Matrix\n3. Inverst Matrix\n4. Matrix inner product\n5. Matrix Transpose\n==========\n6. penjumlahan vector\n7. pengurangan vector\n8. dot product vector\n9. cross product vector");
        printf("\n masukan pilihan (number): ");
        scanf("%i", &input);

        switch (input)
        {
        case 1:
            printf("\n===PEJUMLAHAN MATRIX===\n");
            printf("\nmasukan row/baris = ");
            scanf("%i", &row);
            printf("\nmasukan column = ");
            scanf("%i", &column);

            if (row < 1 && column < 1)
            {
                printf("row dan column tidak boleh dibawah 1");
                break;
            }
            printf("\nMatrix 1 : ");
            createMatrix(&matrix_1, row, column);
            printf("\nMatrix 2 : ");
            createMatrix(&matrix_2, row, column);

            addMatrix(matrix_1, matrix_2, &matrix_result);
            displayMarix(matrix_result);
            break;

        case 2:
            printf("\n===PERKALIAN MATRIX====\n");

            printf("\nmasukan row/baris ke -1 = ");
            scanf("%i", &row);
            printf("\nmasukan column ke -1 =  ");
            scanf("%i", &column);
            createMatrix(&matrix_1, row, column);

            printf("\nmasukan row/baris ke -2 = ");
            scanf("%i", &row2);
            printf("\nmasukan column ke -2 = ");
            scanf("%i", &column2);

            createMatrix(&matrix_2, row2, column2);
            matrix_multiplication(matrix_1, matrix_2, &matrix_result);
            displayMarix(matrix_result);
            break;

        case 3:
            printf("\n==== INVERST MATRIX ====");
            printf("\nmasukan row/baris = ");
            scanf("%i", &row);
            printf("\nmasukan column = ");
            scanf("%i", &column);

            createMatrix(&matrix_1, row, column);
            inverseMatrix(&matrix_1);

            displayMarix(matrix_1);
            break;

        case 4:
            printf("==== INNER PODUCT MATRIX ====");
            printf("\nmasukan row/baris ke -1 = ");
            scanf("%i", &row);
            printf("\nmasukan column ke -1 =  ");
            scanf("%i", &column);
            createMatrix(&matrix_1, row, column);

            printf("\nmasukan row/baris ke -2 = ");
            scanf("%i", &row2);
            printf("\nmasukan column ke -2 = ");
            scanf("%i", &column2);

            createMatrix(&matrix_2, row2, column2);
            result = innerProduct(&matrix_1, &matrix_2);
            if (result == 0)
            {
                printf("\nInner product tidak dapat didapatkan");
                break;
            }
            printf("\nHASIL INNER PRODUCT = %i", result);
            break;

        case 5:
            printf("\n===== MATRIX TRANSPOSE =====");
            printf("\nmasukan row/baris = ");
            scanf("%i", &row);
            printf("\nmasukan column = ");
            scanf("%i", &column);
            createMatrix(&matrix_1, row, column);
            matrixTranspose(matrix_1, &matrix_result);
            displayMarix(matrix_result);
            break;

        case 6:
            printf("\n===== PENJUMLAHAN VECTOR =====");
            printf("\nInput vecor 1 : ");
            createVector(&vector_1);
            printf("\nInput vector 2 :");
            createVector(&vector_2);

            addVector(vector_1, vector_2, &vector_result);
            displayVector(vector_result);

            break;

        case 7:
            printf("\n===== PENGURANGAN VECTOR =====");
            printf("\nInput vecor 1 : ");
            createVector(&vector_1);
            printf("\nInput vector 2 :");
            createVector(&vector_2);

            subVector(vector_1, vector_2, &vector_result);
            displayVector(vector_result);

            break;

        case 8:
            printf("\n===== DOT PRODUCT VECTOR =====");
            printf("\nInput vecor 1 : ");
            createVector(&vector_1);
            printf("\nInput vector 2 :");
            createVector(&vector_2);

            result_vector = dotProductVector(vector_1, vector_2);
            printf("\nHasil DOT PRODUCT = %i ", result_vector);
            break;

        case 9:
            printf("\n===== CROSS PRODUCT VECTOR =====");
            printf("\nInput vecor 1 : ");
            createVector(&vector_1);
            printf("\nInput vector 2 :");
            createVector(&vector_2);

            crossProductVector(vector_1, vector_2, &vector_result);
            displayVector(vector_result);

        case 10:
            printf("\n thank you for using this program, see ya");
            break;

        default:
            printf("\nTolong masukan pilihan yang benar\n");
            isRunning = false;
            break;
            
        }
    

    }   
}