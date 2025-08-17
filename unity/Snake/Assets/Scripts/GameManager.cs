using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance; // singleton pattern
    public int difficulty = 1;

    void Awake()
    {
        // If there is already an instance, destroy duplicates
        if (Instance == null)
        {
            Instance = this;
            DontDestroyOnLoad(gameObject); // keep this object when changing scenes
        }
        else
        {
            Destroy(gameObject);
        }
    }
}
