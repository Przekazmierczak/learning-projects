using UnityEngine;
using System.Collections;

public class AppleSpawner : MonoBehaviour
{
    public GameObject apple;
    private GameObject newApple;
    public Coroutine appleCoroutine;
    private int coordX;
    private int coordY;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        appleCoroutine = StartCoroutine(PerformActionApple());
    }

    IEnumerator PerformActionApple()
    {
        while (true)
        {
            yield return new WaitForSeconds(3f); // wait
            DoActionApple();
        }
    }

    void DoActionApple()
    {
        coordX = Random.Range(-9, 8);
        coordY = Random.Range(-5, 4);
        newApple = Instantiate(apple);
        newApple.transform.position = new Vector2(coordX + 0.5f, coordY + 0.5f);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
